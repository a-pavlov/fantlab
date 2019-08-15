package org.dkfsoft;

import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import io.netty.handler.ssl.SslContext;
import io.netty.handler.ssl.SslContext;
import io.netty.handler.ssl.util.SelfSignedCertificate;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.cli.*;
import org.apache.commons.daemon.Daemon;
import org.apache.commons.daemon.DaemonContext;
import org.apache.commons.daemon.DaemonInitException;
import org.apache.mahout.cf.taste.impl.model.file.FileDataModel;

import java.io.File;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@Slf4j
public class RecommendationDaemon implements Daemon {
    private ServerLauncher serverLauncher;
    private ExecutorService executorService;


    @Override
    public void init(DaemonContext daemonContext) throws DaemonInitException, Exception {
        log.debug("[FL] init");
        Options options = new Options();

        options.addOption("h", "help", false, "display help information");
        options.addOption(OptionBuilder.withLongOpt("port")
                .withDescription("Port for incoming requests")
                .hasArg()
                .withArgName("PORT")
                .create());

        options.addOption(OptionBuilder.withLongOpt("filename")
                .withDescription("Name of file with data model")
                .hasArg()
                .withArgName("FILENAME")
                .create());

        options.addOption(OptionBuilder.withLongOpt("ssl")
                .withDescription("SSL support")
                .withArgName("SSL")
                .create());

        CommandLine line = new PosixParser().parse(options, daemonContext.getArguments());

        if (line.hasOption("h")) {
            HelpFormatter formatter = new HelpFormatter();
            formatter.printHelp("stcpsrv", options);
            System.exit(0);
        }

        if (!line.hasOption("filename")) throw new Exception("filename option required");


        executorService = Executors.newFixedThreadPool(10);

        ObjectMapper objectMapper = new ObjectMapper();
        objectMapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        objectMapper.setSerializationInclusion(NON_NULL);

        FileDataModel dataModel = new FileDataModel(new File(line.getOptionValue("filename")));

        final SslContext sslContext;

        if (line.hasOption("ssl")) {
            SelfSignedCertificate ssc = new SelfSignedCertificate();
            sslContext = SslContext.newServerContext(ssc.certificate(), ssc.privateKey());
        } else {
            sslContext = null;
        }

        serverLauncher = new ServerLauncher(objectMapper, executorService, dataModel, sslContext
                , line.hasOption("port")?Integer.parseInt(line.getOptionValue("port")):(line.hasOption("ssl")?8443:8080));
    }

    @Override
    public void start() throws Exception {
        if (serverLauncher != null) serverLauncher.start();
    }

    @Override
    public void stop() throws Exception {
        if (serverLauncher != null) serverLauncher.closeServer();
    }

    @Override
    public void destroy() {
        if (executorService != null) {
            executorService.shutdown();
        }
    }
}
