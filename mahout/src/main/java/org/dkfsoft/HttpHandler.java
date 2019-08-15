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
import org.dkfsoft.model.WorkMark;
import org.dkfsoft.model.WorkMarkResponse;
import org.omg.CORBA.BAD_PARAM;

import java.nio.charset.StandardCharsets;
import java.util.Collections;
import java.util.List;
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

    private final FileDataModel dataModel;
    private final ObjectMapper objectMapper;

    public HttpHandler(final FileDataModel dataModel, final ObjectMapper objectMapper) {
        this.dataModel = dataModel;
        this.objectMapper = objectMapper;
    }

    private int getIntParam(final String name, QueryStringDecoder queryStringDecoder, int defaultValue) throws FLException {
        assert queryStringDecoder != null;
        assert defaultValue > 0;
        List<String> value = queryStringDecoder.parameters().get(name);

        try {
            return (value != null && !value.isEmpty())?Integer.parseInt(value.get(0)):defaultValue;
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
                    if (queryStringDecoder.parameters() == null || !queryStringDecoder.parameters().containsKey(LOGIN)) {
                        throw new FLException(BAD_REQUEST, "Missed login query parameter");
                    }
                    // add request user id here
                    userId = 14556;
                }

                int neighbors = getIntParam("neighbors", queryStringDecoder, DEFAULT_NEIGHBORS);
                int recommendations = getIntParam("recommendations", queryStringDecoder, DEFAULT_RECOMMENDATIONS);

                List<WorkMark> result = getWorkMarks(userId, neighbors, recommendations);
                WorkMarkResponse workMarkResponse = new WorkMarkResponse(userId, result);
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

    private List<WorkMark> getWorkMarks(long userId, int nearestNeighbors, int recommendsCount) throws FLException {
        try {
            UserSimilarity userSimilarity = new PearsonCorrelationSimilarity(dataModel);
            UserNeighborhood neighborhood = new NearestNUserNeighborhood(nearestNeighbors, userSimilarity, dataModel);
            Recommender recommender = new GenericUserBasedRecommender(dataModel, neighborhood, userSimilarity);
            List<RecommendedItem> recommendations = recommender.recommend(userId, recommendsCount);
            if (recommendations != null)
                return recommendations.stream().map(x -> new WorkMark(x.getItemID(), x.getValue())).collect(Collectors.toList());
            return Collections.emptyList();
        } catch (TasteException e) {
            throw new FLException(INTERNAL_SERVER_ERROR, e.getMessage());
        }
    }
}
