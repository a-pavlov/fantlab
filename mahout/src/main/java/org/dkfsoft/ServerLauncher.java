package org.dkfsoft;

import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
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
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;
import io.netty.handler.ssl.SslContext;
import io.netty.handler.ssl.util.SelfSignedCertificate;
import io.netty.handler.stream.ChunkedWriteHandler;
import lombok.extern.slf4j.Slf4j;
import org.apache.hadoop.ipc.Server;
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
import org.dkfsoft.service.FantlabService;

import java.io.File;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.stream.Collectors;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@Slf4j
public class ServerLauncher extends Thread {
    private final FileDataModel dataModel;
    private final SslContext sslContext;
    private final int port;
    private final ObjectMapper objectMapper;
    private final FantlabService fantlabService;

    public ServerLauncher(final ObjectMapper objectMapper
            , ExecutorService executorService
            , final FileDataModel dataModel
            , final SslContext sslContext
            , int port) {
        this.dataModel = dataModel;
        this.sslContext = sslContext;
        this.port = port;
        this.objectMapper = objectMapper;
        this.fantlabService = new FantlabService(objectMapper, executorService);
    }

    @Override
    public void run() {
        try(NioEventLoopGroup bossGroup = new NioEventLoopGroup();  EventLoopGroup workerGroup = new NioEventLoopGroup()) {
            ServerBootstrap b = new ServerBootstrap();
            b.group(bossGroup, workerGroup)
                    .channel(NioServerSocketChannel.class)
                    .handler(new LoggingHandler(LogLevel.INFO))
                    .childHandler(new ChannelInitializer<SocketChannel>() { // (4)
                        @Override
                        public void initChannel(SocketChannel ch) throws Exception {
                            if (sslContext != null) ch.pipeline().addLast(sslContext.newHandler(ch.alloc()));
                            ch.pipeline()
                                    .addLast("HttpServerCodec", new HttpServerCodec())
                                    .addLast("HttpObjectAggregator", new HttpObjectAggregator(10 * 1024 * 1024))
                                    .addLast("HttpChunkedWrite", new ChunkedWriteHandler())
                                    .addLast("HelloHttpHandler", new HttpHandler(dataModel, objectMapper, fantlabService));
                        }
                    })
                    .option(ChannelOption.SO_BACKLOG, 128)
                    .childOption(ChannelOption.SO_KEEPALIVE, false);

            ChannelFuture f = b.bind(port).sync();
            log.info("server started on port {} data model {}", port, dataModel.getDataFile().getPath());

            f.channel().closeFuture().sync();
        } catch (Exception e) {
            log.error("server error {}", e.getMessage());
        }
    }

    public static void main(String[] args) throws Exception {
        if (args.length != 1) {
            System.out.print("Please provide filename with data");
            return;
        }

        ExecutorService executorService = Executors.newFixedThreadPool(10);

        ObjectMapper objectMapper = new ObjectMapper();
        objectMapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        objectMapper.setSerializationInclusion(NON_NULL);

        FileDataModel dataModel = new FileDataModel(new File(args[0]));
        ServerLauncher serverLauncher = new ServerLauncher(objectMapper, executorService, dataModel, null, 8090);
        serverLauncher.start();
        serverLauncher.join();

        /*
        final SslContext sslCtx;

        if (true) {
            SelfSignedCertificate ssc = new SelfSignedCertificate();
            sslCtx = SslContext.newServerContext(ssc.certificate(), ssc.privateKey());
        } else {
            sslCtx = null;
        }

        try(NioEventLoopGroup bossGroup = new NioEventLoopGroup();  EventLoopGroup workerGroup = new NioEventLoopGroup()) {
            ServerBootstrap b = new ServerBootstrap();
            b.group(bossGroup, workerGroup)
                    .channel(NioServerSocketChannel.class)
                    .handler(new LoggingHandler(LogLevel.INFO))
                    .childHandler(new ChannelInitializer<SocketChannel>() { // (4)
                        @Override
                        public void initChannel(SocketChannel ch) throws Exception {
                            if (sslCtx != null) ch.pipeline().addLast(sslCtx.newHandler(ch.alloc()));
                            ch.pipeline()
                                    .addLast("HttpServerCodec", new HttpServerCodec())
                                    .addLast("HttpObjectAggregator", new HttpObjectAggregator(10 * 1024 * 1024))
                                    .addLast("HttpChunkedWrite", new ChunkedWriteHandler())
                                    .addLast("HelloHttpHandler", new HttpHandler(null, null));
                        }
                    })
                    .option(ChannelOption.SO_BACKLOG, 128)
                    .childOption(ChannelOption.SO_KEEPALIVE, false);

            ChannelFuture f = b.bind(sslCtx!=null?8443:8090).sync();

            f.channel().closeFuture().sync();
        } catch (Exception e) {
            System.out.println("Error " + e.getMessage());
        }
        */
    }
}
