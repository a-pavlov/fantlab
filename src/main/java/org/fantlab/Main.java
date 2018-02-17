package org.fantlab;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.firefox.FirefoxDriver;
import sun.nio.ch.ThreadPool;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.*;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * Created by apavlov on 08.02.18.
 */
@Slf4j
public class Main {

    @AllArgsConstructor
    @Getter
    private static class Tuple<X, Y> {
        private final X first;
        private final Y second;
    }

    static
    {
        System.setProperty("webdriver.gecko.driver", "/home/apavlov/dev/geckodriver");
    }

    private static final int MAX_THREADS_DRIVERS = 5;
    private static final ExecutorService service = Executors.newFixedThreadPool(MAX_THREADS_DRIVERS);

    private static final FLWebDriverCache<WebDriver> webCache = new FLWebDriverCache<WebDriver>(MAX_THREADS_DRIVERS) {
        @Override
        public WebDriver newInstance() {
            return new FirefoxDriver();
        }

        @Override
        public void finalize(WebDriver instance) {
            try {
                instance.close();
            } catch(Exception e) {
                log.warn("close driver error {}", e.getMessage());
            }
        }
    };


    private static class FLUserMarksCallable implements Callable<FLUserMarksCollector> {
        private final FLUserMarksCollector collector;

        public FLUserMarksCallable(final FLUserMarksCollector collector) {
            this.collector = collector;
        }

        @Override
        public FLUserMarksCollector call() throws Exception {
            assert !collector.isFinished();
            return collector.collectUserMarks();
        }
    }

    private static void collectMarks(final Properties prop) throws IOException, FLException {

        String username = prop.getProperty("username");
        String password = prop.getProperty("password");

        if (username == null || password == null) throw new FLException("username or password missed");

        log.info("start with username: \"{}\" and password: \"{}\"", username, password);
        String genreFilename = prop.getProperty("books_genre");
        Set<String> genreDict = new HashSet<>();

        if (genreFilename != null) {
            try (Stream<String> stream = Files.lines(Paths.get(genreFilename))) {
                stream.forEach(x -> genreDict.add(x));
            } catch (Exception e) {
                log.error("reading genres {} error {}", genreFilename, e);
            }
        } else {
            log.info("genre file name missed - do not use genre classifier");
        }

        log.info("genre dictionary size: {}", genreDict.size());
        int maxMarks= prop.getProperty("max_marks")!=null?Integer.parseInt(prop.getProperty("max_marks")):1000;

        WebScraper webSrcapper = new WebScraper(webCache.alloc());

        FLAccum accum = new FLAccum();

        webSrcapper.openFLSite();
        webSrcapper.login(username, password);

        List<FLUserMarksCollector> pendingTasks = webSrcapper.openHLinks()
                .stream()
                .map(x -> new Tuple<String, String>(x.getText(), x.getAttribute("href")))
                .filter(x -> x.getSecond().contains("/user"))
                .map(x -> new FLUserMarksCollector(null, (String) x.getSecond(), maxMarks))
                .limit(prop.getProperty("max_users")!=null?Integer.parseInt(prop.getProperty("max_users")):150)
                .collect(Collectors.toList());

        webCache.free(webSrcapper.getDriver(), true);

        log.info("start scan for {} users", pendingTasks);

        List<Future<FLUserMarksCollector>> runningTasks = new LinkedList<>();

        // loop while we have at least one pending or running task
        while(!pendingTasks.isEmpty() || !runningTasks.isEmpty()) {

            if (!pendingTasks.isEmpty()) {
                // try to start new task
                WebDriver driver = webCache.alloc();

                if (driver != null) {
                    FLUserMarksCollector pendingTask = pendingTasks.get(0);
                    pendingTask.setDriver(driver);
                    Future<FLUserMarksCollector> future = service.submit(new FLUserMarksCallable(pendingTask));
                    runningTasks.add(future);
                    pendingTasks.remove(0);
                } else {
                    try {
                        log.trace("no available drivers, wait");
                        Thread.sleep(5000);
                    } catch (Exception e) {
                        // ignore
                    }
                }
            }

            Iterator<Future<FLUserMarksCollector>> itr = runningTasks.iterator();
            while(itr.hasNext()) {
                Future<FLUserMarksCollector> f = itr.next();

                if (f.isDone()) {
                    try {
                        FLUserMarksCollector fc = f.get();
                        webCache.free(fc.getDriver(), fc.isFinished());

                        // if task is not finished - add it again to pendings tasks list
                        if (fc.isFinished()) {
                            for(final FLUserMarksCollector.Mark m: fc.getMarks()) {
                                final String bookId = FLUtil.link2Name(m.getUrl());
                                if (genreDict.contains(bookId)) accum.addMark(FLUtil.link2Name(fc.getUser()), bookId, m.getValue());
                            }
                        } else {
                            pendingTasks.add(fc);
                        }
                    } catch(ExecutionException e) {
                        log.error("execution exception {}", e.getMessage());
                    } catch(InterruptedException e) {
                        log.error("interrupted exception {}", e.getMessage());
                    } finally {
                        itr.remove();
                    }
                }
            }
        }

        /*for(int i = 0; i < usersCount; ++i) {
            Tuple<String, String> link = links.get(i);

            String user = FLUtil.link2Name((String)link.getSecond());
            log.debug("user {} id {} link {}"
                    , link.getFirst()
                    , user
                    , link.getSecond());
            FLUserMarksCollector umc = new FLUserMarksCollector(webSrcapper.getDriver(), (String) link.getSecond(), maxMarks);
            umc.collectUserMarks();
            log.info("user {} status {}", (String) link.getSecond(), umc.isFinished()?"true":"false");

            List<FLUserMarksCollector.Mark> marks = umc.getMarks();
            for(final FLUserMarksCollector.Mark m: marks) {
                final String bookId = FLUtil.link2Name(m.getUrl());
                if (genreDict.contains(bookId)) accum.addMark(user, bookId, m.getValue());
            }
        }*/

        String fileMarks = prop.getProperty("marks_file");
        String fileUsers = prop.getProperty("users_file");
        String fileBooks = prop.getProperty("books_file");

        if (fileMarks != null) {
            try (PrintWriter writer = new PrintWriter(fileMarks)) {
                for (List<Integer> marks : accum.getMarks()) {
                    writer.write(marks.stream().map(x -> x.toString()).collect(Collectors.joining(",")));
                    writer.write('\n');
                }
            } catch(IOException e) {
                log.error("I/O error on write marks {}", e);
            }
        } else {
            log.warn("marks file is not specified");
        }

        if (fileUsers != null) {
            try(PrintWriter writer = new PrintWriter(fileUsers)) {
                Iterator<String> itr = accum.getUsers().iterator();
                int index = 0;
                while(itr.hasNext()) {
                    writer.write(Integer.toString(index++) + "," + itr.next() + "\n");
                }
            } catch(IOException e) {
                log.error("I/O error on write users {}", e);
            }
        } else {
            log.warn("users file is not specified");
        }

        if (fileBooks != null) {
            try(PrintWriter writer = new PrintWriter(fileBooks)) {
                for(Map.Entry<String, Integer> entry: accum.getBooks().entrySet()) {
                    writer.write(entry.getValue().toString() + "," + entry.getKey() + "\n");
                }
            } catch(IOException e) {
                log.error("I/O error on write books {}", e);
            }
        } else {
            log.warn("books file is not specified");
        }
    }

    private static void collectDictionaryByGenre(final Properties prop) throws IOException, FLException {
        String bgFile = prop.getProperty("books_genre");
        if (bgFile == null) throw new FLException("Property books_genre wasn't provided");

        WebScraper webSrcapper = new WebScraper(webCache.alloc());

        try(PrintWriter writer = new PrintWriter(bgFile)) {
            for (int i = 1; i < 305; ++i) {
                List<String> works = webSrcapper.fetchBooksByGenre("wg1", i);
                if (works.isEmpty()) break;
                works.stream().forEach(x -> writer.write(FLUtil.link2Name(x) + "\n"));
            }
        }
        catch(IOException e) {
            log.error("I/O error on write books by genres {}", e);
        }

        webCache.free(webSrcapper.getDriver(), true);
    }

    public static void main(String args[]) throws IOException {
        try {
            Properties prop = new Properties();
            InputStream input = Main.class.getClassLoader().getResourceAsStream("config.properties");

            if (input == null) throw new FLException("config.properties wasn't found in resources root");
            prop.load(input);

            if (args.length > 0 && args[0].equals("genre")) {
                collectDictionaryByGenre(prop);
            } else {
                collectMarks(prop);
            }

        } catch(FLException e) {
            log.error("FL exception: {}", e.getMessage());
        } catch(IOException e) {
            log.error("IO exception: {}", e);
        } finally {
            service.shutdown();
            webCache.shutdown();
        }
    }
}

