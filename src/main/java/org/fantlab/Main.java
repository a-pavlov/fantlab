package org.fantlab;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.io.FileUtils;
import org.apache.commons.lang3.tuple.Pair;
import org.openqa.selenium.*;
import org.openqa.selenium.firefox.FirefoxDriver;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.stream.Collector;
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

    private static final WebScraper webSrcapper = new WebScraper();

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

        FLAccum accum = new FLAccum();

        webSrcapper.openFLSite();
        webSrcapper.login(username, password);
        webSrcapper.openHLinks();
        List<Tuple<String, String>> links = webSrcapper.openHLinks()
                .stream()
                .map(x -> new Tuple<String, String>(x.getText(), x.getAttribute("href")))
                .filter(x -> x.getSecond().contains("/user"))
                .collect(Collectors.toList());

        int usersCount = Math.max(prop.getProperty("max_users")!=null?Integer.parseInt(prop.getProperty("max_users")):150, links.size());
        int maxMarks= prop.getProperty("max_marks")!=null?Integer.parseInt(prop.getProperty("max_marks")):1000;

        for(int i = 0; i < usersCount; ++i) {
            Tuple<String, String> link = links.get(i);

            String user = FLUtil.link2Name((String)link.getSecond());
            log.debug("user {} id {} link {}"
                    , link.getFirst()
                    , user
                    , link.getSecond());
            List<WebScraper.Mark> marks = webSrcapper.getUserMarks((String) link.getSecond(), maxMarks);
            for(final WebScraper.Mark m: marks) {
                final String bookId = FLUtil.link2Name(m.getUrl());
                if (genreDict.contains(bookId)) accum.addMark(user, bookId, m.getValue());
            }
        }

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
        }

        if (fileBooks != null) {
            try(PrintWriter writer = new PrintWriter(fileBooks)) {
                for(Map.Entry<String, Integer> entry: accum.getBooks().entrySet()) {
                    writer.write(entry.getValue().toString() + "," + entry.getKey() + "\n");
                }
            } catch(IOException e) {
                log.error("I/O error on write books {}", e);
            }
        }
    }

    private static void collectDictionaryByGenre(final Properties prop) throws IOException, FLException {
        String bgFile = prop.getProperty("books_genre");
        if (bgFile == null) throw new FLException("Property books_genre wasn't provided");

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
            webSrcapper.saveScreenshot();
            webSrcapper.closeBrowser();
        }
    }
}

