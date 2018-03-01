package org.fantlab;

import jdk.nashorn.internal.runtime.ECMAErrors;
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

    private static final int MAX_THREADS_DRIVERS = 10;
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
        String myUri = prop.getProperty("my_uri");

        if (username == null || password == null) throw new FLException("username or password missed");
        if (myUri == null) throw new FLException("Self uri is not defined");


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
        int normalUserMaxPages = prop.getProperty("normal_user_max_pages")!=null?Integer.parseInt(prop.getProperty("normal_user_max_pages")):15;

        WebScraper webSrcapper = new WebScraper(webCache.alloc());

        String fileMarks = prop.getProperty("marks_file");
        String fileUsers = prop.getProperty("users_file");
        String fileBooks = prop.getProperty("books_file");

        Map<String, Integer> booksDict = new HashMap<>();

        if (fileBooks != null) {
            try (BufferedReader br = new BufferedReader(new FileReader(fileBooks))) {
                String line;
                while ((line = br.readLine()) != null) {
                    String[] parts = line.split(",");
                    if (parts.length == 2) {
                        booksDict.put(parts[1], Integer.parseInt(parts[0]));
                    }
                }
            } catch(Exception e) {
                log.info("reading books dictionary file failed {}", e.getMessage());
            }
        }

        log.info("load books dictionary size {}", booksDict.size());

        FLAccum accum = new FLAccum(booksDict);

        webSrcapper.openFLSite();
        webSrcapper.login(username, password);

        webCache.free(webSrcapper.getDriver(), true);

        if (myUri != null) {
            log.info("collect self marks in single mode to make sure we have own marks on first place");
            List<FLUserMarksCollector> myList = new LinkedList<>();
            myList.add(new FLUserMarksCollector(null, myUri, maxMarks, genreDict));
            processTasks(myList, webCache, genreDict, accum);
        }

        processTasks(webSrcapper.openHLinks()
                .stream()
                .map(x -> new Tuple<String, String>(x.getText(), x.getAttribute("href")))
                .filter(x -> x.getSecond().contains("/user"))
                .map(x -> new FLUserMarksCollector(null, (String) x.getSecond(), maxMarks, genreDict))
                .limit(prop.getProperty("max_users")!=null?Integer.parseInt(prop.getProperty("max_users")):150)
                .collect(Collectors.toList()), webCache, genreDict, accum);

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

    private static void processTasks(final List<FLUserMarksCollector> pendingTasks
            , final FLWebDriverCache<WebDriver> webCache
            , final Set<String> genreDict
            , final FLAccum accum) {

        log.info("process tasks {} started", pendingTasks.size());

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

                        if (fc.isFinished()) {
                            for(final FLUserMarksCollector.Mark m: fc.getMarks()) {
                                final String bookId = FLUtil.link2Name(m.getUrl());
                                accum.addMark(FLUtil.link2Name(fc.getUser()), bookId, m.getValue());
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

        log.info("process tasks completed");
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

    private static void collectUserData(Properties prop) throws FLException {
        String myUri = prop.getProperty("my_uri");
        if (myUri == null) throw new FLException("Self uri is not defined");
        FLUserProfileCollector uc = new FLUserProfileCollector(webCache.alloc(), myUri);
        uc.collectUserProfileData();
        log.info("collected {}", uc.getUserData().toString());
        webCache.free(uc.getDriver(), true);
    }

    public static void main(String args[]) throws IOException {
        try {
            Properties prop = new Properties();
            InputStream input = Main.class.getClassLoader().getResourceAsStream("config.properties");

            if (input == null) throw new FLException("config.properties wasn't found in resources root");
            prop.load(input);

            final String gecko = prop.getProperty("webdriver_gecko_driver");
            if (gecko == null) new FLException("webdriver_gecko_driver is not specified in properties");
            System.setProperty("webdriver.gecko.driver", gecko);

            if (args.length > 0) {
                if (args[0].equals("genre")) {
                    collectDictionaryByGenre(prop);
                } else if (args[0].equals("user")) {
                    collectUserData(prop);
                }
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

