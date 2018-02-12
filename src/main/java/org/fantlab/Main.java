package org.fantlab;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.io.FileUtils;
import org.apache.commons.lang3.tuple.Pair;
import org.openqa.selenium.*;
import org.openqa.selenium.firefox.FirefoxDriver;

import java.io.*;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.stream.Collector;
import java.util.stream.Collectors;

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

        FLAccum accum = new FLAccum();
        System.setProperty("webdriver.gecko.driver", "/home/apavlov/dev/geckodriver");

        webSrcapper.openFLSite();
        webSrcapper.login(username, password);
        webSrcapper.openHLinks();
        List<Tuple<String, String>> links = webSrcapper.openHLinks()
                .stream()
                .map(x -> new Tuple<String, String>(x.getText(), x.getAttribute("href")))
                .collect(Collectors.toList());

        int i = 3;
        for(Tuple link: links) {
            String user = FLUtil.link2Name((String)link.getSecond());
            log.info("user {} id {} link {}"
                    , link.getFirst()
                    , user
                    , link.getSecond());
            List<WebScraper.Mark> marks = webSrcapper.getUserMarks((String) link.getSecond());
            for(final WebScraper.Mark m: marks) {
                //log.info("add mark {}", m);
                accum.addMark(user, FLUtil.link2Name(m.getUrl()), m.getValue());
            }

            if (i-- == 0) break;
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
            for (int i = 301; i < 305; ++i) {
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

