package org.dkfsoft;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.handler.codec.http.*;
import lombok.extern.slf4j.Slf4j;
import org.apache.mahout.cf.taste.common.TasteException;
import org.apache.mahout.cf.taste.impl.model.file.FileDataModel;
import org.apache.mahout.cf.taste.impl.neighborhood.NearestNUserNeighborhood;
import org.apache.mahout.cf.taste.impl.neighborhood.ThresholdUserNeighborhood;
import org.apache.mahout.cf.taste.impl.recommender.GenericUserBasedRecommender;
import org.apache.mahout.cf.taste.impl.similarity.*;
import org.apache.mahout.cf.taste.neighborhood.UserNeighborhood;
import org.apache.mahout.cf.taste.recommender.RecommendedItem;
import org.apache.mahout.cf.taste.recommender.Recommender;
import org.apache.mahout.cf.taste.similarity.UserSimilarity;
import org.dkfsoft.model.Work;
import org.dkfsoft.model.WorkMark;
import org.dkfsoft.model.WorkMarkResponse;
import org.dkfsoft.service.FantlabService;

import java.nio.charset.StandardCharsets;
import java.util.*;
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
    public static Pattern LOGIN_PATTERN = Pattern.compile("^\\/recommendation\\/{0,1}$");

    private static final int DEFAULT_NEIGHBORS = 20;
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
                log.info("http request: {} {}", request.method(), request.uri());

                final String path = queryStringDecoder.path();

                String content;
                String contentType;

                if (path.equals("/")) {
                    content = new Scanner( HttpHandler.class.getResourceAsStream("/index.html")).useDelimiter("\\A").next();
                    contentType = "text/html";
                } else {
                    Matcher userMatcher = USER_PATTERN.matcher(path != null ? path : "");
                    Matcher loginMatcher = LOGIN_PATTERN.matcher(path != null ? path : "");

                    boolean userMatch = userMatcher.find();
                    boolean loginMatch = loginMatcher.find();

                    if (!userMatch && !loginMatch) {
                        throw new FLException(NOT_FOUND, "Incorrect path");
                    }

                    long userId;

                    if (userMatch) {
                        try {
                            userId = Long.parseLong(userMatcher.group(2));
                        } catch (NumberFormatException e) {
                            // never happened due to pattern
                            throw new FLException(BAD_REQUEST, "ID is not a number: " + userMatcher.group(2));
                        }
                    } else {
                        if (queryStringDecoder.parameters() == null || !queryStringDecoder.parameters().containsKey(LOGIN) || queryStringDecoder.parameters().get(LOGIN).isEmpty()) {
                            throw new FLException(BAD_REQUEST, "Missed or empty login query parameter");
                        }
                        // add request user id here
                        userId = fantlabService.getUserIdByLogin(queryStringDecoder.parameters().get(LOGIN).get(0)).getUser_id();

                        if (userId == 0) {
                            throw new FLException(NOT_FOUND, "User with specified login: " + queryStringDecoder.parameters().get(LOGIN).get(0) + " not found");
                        }
                    }

                    final int neighbors = getIntParam("neighbors", queryStringDecoder, DEFAULT_NEIGHBORS, 1, 500);
                    final int recommendations = getIntParam("recommendations", queryStringDecoder, DEFAULT_RECOMMENDATIONS, 1, 200);
                    final int genre = getIntParam("genre", queryStringDecoder, DEFAULT_GENRE, 0, 20);

                    log.info("[rec] start recommendations");
                    List<RecommendedItem> recommendsLL = getWorkMarks(userId, neighbors, recommendations, new LogLikelihoodSimilarity(dataModel));
                    log.info("[rec] calculation LogLikelihoodSimilarity done");
                    List<RecommendedItem> recommendsCityBlock = getWorkMarks(userId, neighbors, recommendations, new CityBlockSimilarity(dataModel));
                    log.info("[rec] calculation CityBlockSimilarity done");

                    Set<Long> distinct = new LinkedHashSet<>();
                    distinct.addAll(recommendsLL.stream().map(x -> x.getItemID()).collect(Collectors.toList()));
                    distinct.addAll(recommendsCityBlock.stream().map(x -> x.getItemID()).collect(Collectors.toList()));
                    log.info("total distict works: {}", distinct.size());
                    List<Work> works = fantlabService.getWorkDetails(distinct.stream().collect(Collectors.toList()));

                    Map<Long, Float> recommendDictLL = recommendsLL.stream().collect(Collectors.toMap(RecommendedItem::getItemID, RecommendedItem::getValue));
                    Map<Long, Float> recommendDictCB = recommendsCityBlock.stream().collect(Collectors.toMap(RecommendedItem::getItemID, RecommendedItem::getValue));

                    WorkMarkResponse workMarkResponse = new WorkMarkResponse(userId
                            , works.stream().filter(x -> x.isGenre(genre)).map(x -> new WorkMark(x.getWork_id()
                            , recommendDictLL.containsKey(x.getWork_id())?recommendDictLL.get(x.getWork_id()):0.0f
                            , recommendDictCB.containsKey(x.getWork_id())?recommendDictCB.get(x.getWork_id()):0.0f
                            , x.getWork_name()
                            , x.getWork_description()
                            , x.getWork_year()
                            , x.getAuthors().stream().map(a -> a.getName()).collect(Collectors.joining(", ")).toString()
                            , x.getMainGenre()))
                            .collect(Collectors.toList()));
                    try {
                        content = objectMapper.writeValueAsString(workMarkResponse);
                    } catch (JsonProcessingException e) {
                        throw new FLException(INTERNAL_SERVER_ERROR, e.getMessage());
                    }

                    contentType = "application/json";
                }


                if (HttpHeaderUtil.is100ContinueExpected(request)) {
                    ctx.write(new DefaultFullHttpResponse(HTTP_1_1, CONTINUE));
                }

                boolean keepAlive = HttpHeaderUtil.isKeepAlive(request);

                FullHttpResponse fullHttpResponse = new DefaultFullHttpResponse(HTTP_1_1, OK, Unpooled.copiedBuffer(content, StandardCharsets.UTF_8));
                fullHttpResponse.headers().set(CONTENT_TYPE, contentType);
                fullHttpResponse.headers().set(CONTENT_LENGTH, Integer.toString(fullHttpResponse.content().readableBytes()));

                if (!keepAlive) {
                    ctx.write(fullHttpResponse).addListener(ChannelFutureListener.CLOSE);
                } else {
                    fullHttpResponse.headers().set(CONNECTION, KEEP_ALIVE);
                    ctx.write(fullHttpResponse);
                }
            } catch (FLException e) {
                log.error("request processing error code {} details {}"
                        , e.getStatus()
                        , e.getMessage());
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

    private List<RecommendedItem> getWorkMarks(long userId, int nearestNeighbors, int recommendsCount, UserSimilarity userSimilarity) throws FLException {
        try {
            //UserSimilarity userSimilarity = new LogLikelihoodSimilarity(dataModel);
            //UserSimilarity userSimilarity = new CityBlockSimilarity(dataModel);
            //UserSimilarity userSimilarity = new SpearmanCorrelationSimilarity(dataModel);
            //UserNeighborhood neighborhood = new ThresholdUserNeighborhood(0.5, userSimilarity, dataModel);
            log.info("[rec] start neighborhood");
            UserNeighborhood neighborhood = new NearestNUserNeighborhood(50, userSimilarity, dataModel);
            log.info("[rec] finish neighborhood");
            //long neighbors[] = neighborhood.getUserNeighborhood(userId);
            //log.info("total sim {}", neighbors.length);
            //for(int i = 0; i< neighbors.length; ++i) {
            //    log.info("{} sim: {}", neighbors[i], userSimilarity.userSimilarity(neighbors[i], userId));
            //}

            Recommender recommender = new GenericUserBasedRecommender(dataModel, neighborhood, userSimilarity);
            return recommender.recommend(userId, recommendsCount);
        } catch (TasteException e) {
            throw new FLException(INTERNAL_SERVER_ERROR, e.getMessage());
        }
    }
}
