package org.dkfsoft;

import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.handler.codec.http.*;
import org.apache.hadoop.mapred.JobHistory;

import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static io.netty.handler.codec.http.HttpHeaderNames.CONNECTION;
import static io.netty.handler.codec.http.HttpHeaderNames.CONTENT_LENGTH;
import static io.netty.handler.codec.http.HttpHeaderNames.CONTENT_TYPE;
import static io.netty.handler.codec.http.HttpHeaderValues.KEEP_ALIVE;
import static io.netty.handler.codec.http.HttpResponseStatus.*;
import static io.netty.handler.codec.http.HttpVersion.HTTP_1_1;

public class HttpHandler extends ChannelInboundHandlerAdapter {
    private static final String RECOM = "recommendation";
    private static final String USER = "user";
    private static final String LOGIN = "login";

    public static Pattern USER_PATTERN = Pattern.compile("(^\\/recommendation\\/user\\/)([0-9]+$)");
    public static Pattern LOGIN_PATTERN = Pattern.compile("^\\/recommendation\\/login\\/{0,1}$");

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) {
        if (msg instanceof HttpRequest) {
            try {
                HttpRequest request = (HttpRequest) msg;

                QueryStringDecoder queryStringDecoder = new QueryStringDecoder(request.uri());

                System.out.println("uri: " + queryStringDecoder.path() + " query " + queryStringDecoder.parameters());
                //if (queryStringDecoder.path().equals(""))
                System.out.println("http request " + request.toString());

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

                if (HttpHeaderUtil.is100ContinueExpected(request)) {
                    ctx.write(new DefaultFullHttpResponse(HTTP_1_1, CONTINUE));
                }

                boolean keepAlive = HttpHeaderUtil.isKeepAlive(request);
                FullHttpResponse response = new DefaultFullHttpResponse(HTTP_1_1, OK, Unpooled.copiedBuffer("{\"id\": 1, \"status\": \"ok\"}", StandardCharsets.UTF_8));
                response.headers().set(CONTENT_TYPE, "application/json");
                response.headers().set(CONTENT_LENGTH, Integer.toString(response.content().readableBytes()));

                if (!keepAlive) {
                    ctx.write(response).addListener(ChannelFutureListener.CLOSE);
                } else {
                    response.headers().set(CONNECTION, KEEP_ALIVE);
                    ctx.write(response);
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
}
