package org.fantlab;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.firefox.FirefoxDriver;
import org.openqa.selenium.phantomjs.PhantomJSDriver;

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
@Getter
public class FLMain {

    @AllArgsConstructor
    @Getter
    private static class Tuple<X, Y> {
        private final X first;
        private final Y second;
    }

    private final ExecutorService service;

    private final FLWebDriverCache<WebDriver> webCache;

    public FLMain(boolean gecko, int maxThreads) {
        if (gecko) {
            webCache = new FLWebDriverCache<WebDriver>(maxThreads) {
                @Override
                public WebDriver newInstance() {
                    WebDriver driver = new FirefoxDriver();
                    //driver.manage().timeouts().implicitlyWait(30, TimeUnit.SECONDS);
                    return driver;
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
        } else {
            webCache = new FLWebDriverCache<WebDriver>(maxThreads) {
                @Override
                public WebDriver newInstance() {
                    WebDriver driver = new PhantomJSDriver();
                    return driver;
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
        }

        service = Executors.newFixedThreadPool(maxThreads);
    }

    private void collectMarks(final Properties prop) throws IOException, FLException {

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

        FLAccum accum = new FLAccum();

        webSrcapper.openFLSite();
        webSrcapper.login(username, password);

        webCache.free(webSrcapper.getDriver(), true);

        if (myUri != null) {
            log.info("collect self marks in single mode to make sure we have own marks on first place");
            List<FLCollector> myList = new LinkedList<>();
            myList.add(new FLUserMarksCollector(null, myUri, maxMarks, genreDict, 1000));
            processTasks(myList, webCache, accum);
        }


        List<Tuple<String, String>> sourceUsers = webSrcapper
                .openHLinks()
                .stream()
                .map(x -> new Tuple<String, String>(x.getText(), x.getAttribute("href")))
                .filter(x -> x.getSecond().contains("/user"))
                .collect(Collectors.toList());

        log.info("source users count {}", sourceUsers.size());

        processTasks(sourceUsers.stream().map(x -> new FLUserProfileCollector(null, x.second)).collect(Collectors.toList())
                , webCache
                , accum);


        int validUserMaxMarks = prop.getProperty("valid_user_max_marks")!=null?Integer.parseInt(prop.getProperty("valid_user_max_marks")):6000;

        // process users only valid users
        processTasks(sourceUsers.stream()
                .filter(x -> accum.getUsersData().containsKey(x.second) && accum.getUsersData().get(x.second).getMarks() < validUserMaxMarks)
                .limit(prop.getProperty("max_users")!=null?Integer.parseInt(prop.getProperty("max_users")):150)
                .map(x -> new FLUserMarksCollector(null, x.second, maxMarks, genreDict, accum.getUsersData().get(x.second).getMarks()))
                .collect(Collectors.toList())
                , webCache, accum);

        /*processTasks(webSrcapper.openHLinks()
                .stream()
                .map(x -> new Tuple<String, String>(x.getText(), x.getAttribute("href")))
                .filter(x -> x.getSecond().contains("/user"))
                .map(x -> new FLUserMarksCollector(null, (String) x.getSecond(), maxMarks, genreDict))
                .limit(prop.getProperty("max_users")!=null?Integer.parseInt(prop.getProperty("max_users")):150)
                .collect(Collectors.toList()), webCache, accum);
                */

        if (fileMarks != null) {
            try (PrintWriter writer = new PrintWriter(fileMarks)) {
                int totalMarks = accum.getTotalMarks();
                for(int i = 0; i < totalMarks; i++) {
                    writer.write(accum.getMarksByIndex(i).stream().map(x -> x.toString()).collect(Collectors.joining(",")));
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
            // ok, sort books in position order
            String sortedByPosition[] = new String[accum.getBooks().size()];
            Arrays.fill(sortedByPosition, null);
            accum.getBooks().entrySet().stream().forEach(x -> sortedByPosition[x.getValue()] = x.getKey());
            for(int i = 0; i < sortedByPosition.length; ++i) assert sortedByPosition[i] != null;

            try(PrintWriter writer = new PrintWriter(fileBooks)) {
                for(int i = 0; i < sortedByPosition.length; ++i) {
                    writer.write(String.format("%d,%s\n", i, sortedByPosition[i]));
                }
            } catch(IOException e) {
                log.error("I/O error on write books {}", e);
            }
        } else {
            log.warn("books file is not specified");
        }
    }

    private void processTasks(final List<FLCollector> pendingTasks
            , final FLWebDriverCache<WebDriver> webCache
            , final FLAccum accum) {

        log.info("process tasks {} started", pendingTasks.size());

        List<Future<FLCollector>> runningTasks = new LinkedList<>();

        // loop while we have at least one pending or running task
        while(!pendingTasks.isEmpty() || !runningTasks.isEmpty()) {

            if (!pendingTasks.isEmpty()) {
                // try to start new task
                WebDriver driver = webCache.alloc();

                if (driver != null) {
                    FLCollector pendingTask = pendingTasks.get(0);
                    pendingTask.setDriver(driver);
                    Future<FLCollector> future = service.submit(new FLCallable(pendingTask));
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

            Iterator<Future<FLCollector>> itr = runningTasks.iterator();
            while(itr.hasNext()) {
                Future<FLCollector> f = itr.next();

                if (f.isDone()) {
                    try {
                        FLCollector fc = f.get();
                        webCache.free(fc.getDriver(), fc.isFinished());

                        if (fc.isFinished()) {
                            fc.storeResuls(accum);
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

    private void collectDictionaryByGenre(final Properties prop) throws IOException, FLException {
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

    private void collectUserData(Properties prop) throws FLException {
        String myUri = prop.getProperty("my_uri");
        if (myUri == null) throw new FLException("Self uri is not defined");
        FLUserProfileCollector uc = new FLUserProfileCollector(webCache.alloc(), myUri);
        uc.collect();
        log.info("collected {}", uc.getUserData().toString());
        webCache.free(uc.getDriver(), true);
    }

    public static void main(String args[]) throws IOException {
        FLMain fl = null;

        try {
            Properties prop = new Properties();
            InputStream input = FLMain.class.getClassLoader().getResourceAsStream("config.properties");

            if (input == null) throw new FLException("config.properties wasn't found in resources root");
            prop.load(input);

            final String gecko = prop.getProperty("webdriver_gecko_driver");
            final String phantomjs = prop.getProperty("webdriver_phantomjs_driver");

            int maxThreads = prop.getProperty("max_threads")!=null?Integer.parseInt(prop.getProperty("max_threads")):4;

            if (gecko == null && phantomjs == null) new FLException("No driver specified in properties");

            if (gecko != null) {
                System.setProperty("webdriver.gecko.driver", gecko);
                fl = new FLMain(true, maxThreads);

            } else {
                System.setProperty("phantomjs.binary.path", phantomjs);
                fl = new FLMain(false, maxThreads);
            }

            if (args.length > 0) {
                if (args[0].equals("genre")) {
                    fl.collectDictionaryByGenre(prop);
                } else if (args[0].equals("user")) {
                    fl.collectUserData(prop);
                }
            } else {
                fl.collectMarks(prop);
            }

        } catch(FLException e) {
            log.error("FL exception: {}", e.getMessage());
        } catch(IOException e) {
            log.error("IO exception: {}", e);
        } finally {
            if (fl != null) {
                fl.getService().shutdown();
                fl.getWebCache().shutdown();
            }
        }
    }
}

