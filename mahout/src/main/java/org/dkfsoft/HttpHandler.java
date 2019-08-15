package org.dkfsoft;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.handler.codec.http.*;
import lombok.extern.slf4j.Slf4j;
import org.apache.hadoop.mapred.JobHistory;
import org.apache.mahout.cf.taste.common.TasteException;
import org.apache.mahout.cf.taste.impl.model.file.FileDataModel;
import org.apache.mahout.cf.taste.impl.neighborhood.NearestNUserNeighborhood;
import org.apache.mahout.cf.taste.impl.recommender.GenericUserBasedRecommender;
import org.apache.mahout.cf.taste.impl.similarity.PearsonCorrelationSimilarity;
import org.apache.mahout.cf.taste.neighborhood.UserNeighborhood;
import org.apache.mahout.cf.taste.recommender.RecommendedItem;
import org.apache.mahout.cf.taste.recommender.Recommender;
import org.apache.mahout.cf.taste.similarity.UserSimilarity;
import org.dkfsoft.model.Work;
import org.dkfsoft.model.WorkMark;
import org.dkfsoft.model.WorkMarkResponse;
import org.dkfsoft.service.FantlabService;
import org.omg.CORBA.BAD_PARAM;

import java.nio.charset.StandardCharsets;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

import static io.netty.handler.codec.http.HttpHeaderNames.CONNECTION;
import static io.netty.handler.codec.http.HttpHeaderNames.CONTENT_LENGTH;
import static io.netty.handler.codec.http.HttpHeaderNames.CONTENT_TYPE;
import static io.netty.handler.codec.http.HttpHeaderValues.KEEP_ALIVE;
import static io.netty.handler.codec.http.HttpResponseStatus.*;
import static io.netty.handler.codec.http.HttpVersion.HTTP_1_1;

@Slf4j
public class HttpHandler extends ChannelInboundHandlerAdapter {
    private static final String RECOM = "recommendation";
    private static final String USER = "user";
    private static final String LOGIN = "login";

    public static Pattern USER_PATTERN = Pattern.compile("(^\\/recommendation\\/user\\/)([0-9]+$)");
    public static Pattern LOGIN_PATTERN = Pattern.compile("^\\/recommendation\\/login\\/{0,1}$");

    private static final int DEFAULT_NEIGHBORS = 100;
    private static final int DEFAULT_RECOMMENDATIONS = 100;
    private static final int DEFAULT_GENRE = 0;

    private final FileDataModel dataModel;
    private final ObjectMapper objectMapper;
    private final FantlabService fantlabService;

    public HttpHandler(final FileDataModel dataModel, final ObjectMapper objectMapper, final FantlabService fantlabService) {
        this.dataModel = dataModel;
        this.objectMapper = objectMapper;
        this.fantlabService = fantlabService;
    }

    private int getIntParam(final String name, QueryStringDecoder queryStringDecoder, int defaultValue, int minValue, int maxValue) throws FLException {
        assert queryStringDecoder != null;
        List<String> value = queryStringDecoder.parameters().get(name);
        assert minValue < maxValue;

        try {
            int result = (value != null && !value.isEmpty())?Integer.parseInt(value.get(0)):defaultValue;
            if (result < minValue || result > maxValue) throw new FLException(BAD_REQUEST, "Parameter value out of bounds");
            return result;
        } catch (NumberFormatException e) {
            throw new FLException(BAD_REQUEST, "Invalid parameter " + name + " : " + value.get(0));
        }
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) {
        if (msg instanceof HttpRequest) {
            try {
                HttpRequest request = (HttpRequest) msg;

                QueryStringDecoder queryStringDecoder = new QueryStringDecoder(request.uri());
                log.debug("http request {}", request.toString());

                final String path = queryStringDecoder.path();

                Matcher userMatcher = USER_PATTERN.matcher(path!=null?path:"");
                Matcher loginMatcher = LOGIN_PATTERN.matcher(path!=null?path:"");

                boolean userMatch = userMatcher.find();
                boolean loginMatch = loginMatcher.find();

                if (!userMatch && !loginMatch) {
                    throw new FLException(NOT_FOUND, "Incorrect path");
                }

                long userId = 0;

                if (userMatch) {
                    userId = Long.parseLong(userMatcher.group(2));
                } else {
                    if (queryStringDecoder.parameters() == null || !queryStringDecoder.parameters().containsKey(LOGIN) || queryStringDecoder.parameters().get(LOGIN).isEmpty()) {
                        throw new FLException(BAD_REQUEST, "Missed or empty login query parameter");
                    }
                    // add request user id here
                    userId = fantlabService.getUserIdByLogin(queryStringDecoder.parameters().get(LOGIN).get(0)).getUser_id();
                }

                final int neighbors = getIntParam("neighbors", queryStringDecoder, DEFAULT_NEIGHBORS, 1, 500);
                final int recommendations = getIntParam("recommendations", queryStringDecoder, DEFAULT_RECOMMENDATIONS, 1, 200);
                final int genre = getIntParam("genre", queryStringDecoder, DEFAULT_GENRE, 0, 20);

                List<RecommendedItem> recommends = getWorkMarks(userId, neighbors, recommendations);

                Map<Long, Float> recommendDict = recommends.stream().collect(Collectors.toMap(RecommendedItem::getItemID, RecommendedItem::getValue));
                List<Work> works = fantlabService.getWorkDetails(recommends.stream().map(x -> x.getItemID()).collect(Collectors.toList()));

                WorkMarkResponse workMarkResponse = new WorkMarkResponse(userId, works.stream().filter(x -> x.isGenre(genre)).map(x -> new WorkMark(x.getWork_id(), recommendDict.get(x.getWork_id()), x.getWork_name(), x.getWork_description())).collect(Collectors.toList()));
                String content;

                try {
                    content = objectMapper.writeValueAsString(workMarkResponse);
                } catch (JsonProcessingException e) {
                    throw new FLException(INTERNAL_SERVER_ERROR, e.getMessage());
                }

                if (HttpHeaderUtil.is100ContinueExpected(request)) {
                    ctx.write(new DefaultFullHttpResponse(HTTP_1_1, CONTINUE));
                }

                boolean keepAlive = HttpHeaderUtil.isKeepAlive(request);

                FullHttpResponse fullHttpResponse = new DefaultFullHttpResponse(HTTP_1_1, OK, Unpooled.copiedBuffer(content, StandardCharsets.UTF_8));
                fullHttpResponse.headers().set(CONTENT_TYPE, "application/json");
                fullHttpResponse.headers().set(CONTENT_LENGTH, Integer.toString(fullHttpResponse.content().readableBytes()));

                if (!keepAlive) {
                    ctx.write(fullHttpResponse).addListener(ChannelFutureListener.CLOSE);
                } else {
                    fullHttpResponse.headers().set(CONNECTION, KEEP_ALIVE);
                    ctx.write(fullHttpResponse);
                }
            } catch (FLException e) {
                FullHttpResponse response = new DefaultFullHttpResponse(HTTP_1_1, e.getStatus()
                        , Unpooled.copiedBuffer("{\"status\": \"fail\", \"message\": \"" + e.getMessage() + "\"}", StandardCharsets.UTF_8));
                response.headers().set(CONTENT_TYPE, "application/json");
                response.headers().set(CONTENT_LENGTH, Integer.toString(response.content().readableBytes()));
                ctx.write(response);
            }
        }
    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) {
        ctx.flush();
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        //cause.printStackTrace();
        //ctx.close();

        super.exceptionCaught(ctx, cause);
        ctx.writeAndFlush(new DefaultFullHttpResponse(HTTP_1_1, HttpResponseStatus.INTERNAL_SERVER_ERROR));
    }

    private List<RecommendedItem> getWorkMarks(long userId, int nearestNeighbors, int recommendsCount) throws FLException {
        try {
            UserSimilarity userSimilarity = new PearsonCorrelationSimilarity(dataModel);
            UserNeighborhood neighborhood = new NearestNUserNeighborhood(nearestNeighbors, userSimilarity, dataModel);
            Recommender recommender = new GenericUserBasedRecommender(dataModel, neighborhood, userSimilarity);
            return recommender.recommend(userId, recommendsCount);
        } catch (TasteException e) {
            throw new FLException(INTERNAL_SERVER_ERROR, e.getMessage());
        }
    }
}
