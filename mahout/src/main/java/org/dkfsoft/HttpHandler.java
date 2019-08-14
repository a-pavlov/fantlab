package org.dkfsoft;

import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.handler.codec.http.*;
import org.apache.hadoop.mapred.JobHistory;

import java.nio.charset.StandardCharsets;

import static io.netty.handler.codec.http.HttpHeaderNames.CONNECTION;
import static io.netty.handler.codec.http.HttpHeaderNames.CONTENT_LENGTH;
import static io.netty.handler.codec.http.HttpHeaderNames.CONTENT_TYPE;
import static io.netty.handler.codec.http.HttpHeaderValues.KEEP_ALIVE;
import static io.netty.handler.codec.http.HttpResponseStatus.CONTINUE;
import static io.netty.handler.codec.http.HttpResponseStatus.OK;
import static io.netty.handler.codec.http.HttpVersion.HTTP_1_1;

public class HttpHandler extends ChannelInboundHandlerAdapter {

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) {
        if (msg instanceof HttpRequest) {
            HttpRequest request = (HttpRequest) msg;
            System.out.println("http request " + request.toString());

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
        }
    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) {
        ctx.flush();
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        super.exceptionCaught(ctx, cause);
        ctx.writeAndFlush(new DefaultFullHttpResponse(HTTP_1_1, HttpResponseStatus.INTERNAL_SERVER_ERROR));
    }
}
