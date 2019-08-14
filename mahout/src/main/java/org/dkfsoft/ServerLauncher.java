package org.dkfsoft;

import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import io.netty.handler.codec.http.HttpObjectAggregator;
import io.netty.handler.codec.http.HttpServerCodec;
import io.netty.handler.stream.ChunkedWriteHandler;

public class ServerLauncher {
    public static void main(String[] args) throws Exception {
        try(NioEventLoopGroup bossGroup = new NioEventLoopGroup();  EventLoopGroup workerGroup = new NioEventLoopGroup()) {
            ServerBootstrap b = new ServerBootstrap();
            b.group(bossGroup, workerGroup)
                    .channel(NioServerSocketChannel.class)
                    .childHandler(new ChannelInitializer<SocketChannel>() { // (4)
                        @Override
                        public void initChannel(SocketChannel ch) throws Exception {
                            ch.pipeline()
                                    .addLast("HttpServerCodec", new HttpServerCodec())
                                    .addLast("HttpObjectAggregator", new HttpObjectAggregator(10 * 1024 * 1024))
                                    .addLast("HttpChunkedWrite", new ChunkedWriteHandler())
                                    .addLast("HelloHttpHandler", new HttpHandler());
                        }
                    })
                    .option(ChannelOption.SO_BACKLOG, 128)
                    .childOption(ChannelOption.SO_KEEPALIVE, false);

            ChannelFuture f = b.bind(8090).sync();

            f.channel().closeFuture().sync();
        } catch (Exception e) {
            System.out.println("Error " + e.getMessage());
        }
    }
}
