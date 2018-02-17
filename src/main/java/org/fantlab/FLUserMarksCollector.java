package org.fantlab;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.Getter;
import lombok.ToString;
import lombok.extern.slf4j.Slf4j;
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;

import java.util.LinkedList;
import java.util.List;

/**
 * Created by apavlov on 15.02.18.
 */
@Slf4j
@Getter
public class FLUserMarksCollector {
    @Data
    @AllArgsConstructor
    @ToString
    public static class Mark {
        public String url;
        public int value;
    }

    private static final String USER_MARKS_ALL_FORMAT = "%s/markspage%d";
    private static final int MAX_PAGES = 1000;

    List<Mark> marks = new LinkedList<>();

    final String user;
    final int maxMarks;

    /**
     * web driver as is
     */
    private WebDriver driver;

    /**
     * collection finished or not
     */
    boolean finished = false;

    /**
     *  current page
     */
    int page = 1;

    public FLUserMarksCollector(final WebDriver driver
            , final String user
            , int maxMarks) {
        this.driver = driver;
        this.user = user;
        this.maxMarks = maxMarks;
    }

    public void setDriver(final WebDriver driver) {
        assert driver != null;
        this.driver = driver;
    }

    /**
     * fetch marks from one single page
     * add all marks or nothing
     * @param pageUrl
     * @param marks
     * @param maxMarks
     */
    private void getUserMarksFromPage(final String pageUrl, List<Mark> marks, int maxMarks) {
        driver.navigate().to(pageUrl);
        List<WebElement> table = driver.findElements(By.xpath("/html/body/div[3]/div/div/div[2]/main/table[1]/tbody/tr"));

        log.debug("navigate to {} table size {}"
                , pageUrl
                , table.size());

        List<Mark> localMarks = new LinkedList<>();

        for(int i = 1; i < table.size(); ++i) {
            WebElement tr = table.get(i);
            List<WebElement> tds = tr.findElements(By.tagName("td"));

            if (tds.size() < 2) {
                break;
            }

            int mark = 0;

            try {
                mark = Integer.parseInt(tds.get(1).getText());
            } catch (NumberFormatException e) {
               log.warn("hmm mark is not integer");
            }

            if (mark > 0) {
                WebElement tda = tds.get(0).findElement(By.tagName("a"));
                if (tda != null) {
                    localMarks.add(new Mark(tda.getAttribute("href"), mark));
                }
            }

            if (marks.size() + localMarks.size() >= maxMarks) break;
        }

        marks.addAll(localMarks);
    }

    public FLUserMarksCollector collectUserMarks() {
        assert user != null;
        for(; page < MAX_PAGES; ++page) {
            String pageUrl = String.format(USER_MARKS_ALL_FORMAT, user, page);

            boolean roundCompleted = false;

            for(int iteration = 0; iteration < 2; ++iteration) {
                try {
                    int beforeMarks = marks.size();
                    getUserMarksFromPage(pageUrl, marks, maxMarks);
                    finished = (beforeMarks == marks.size() || marks.size() >= maxMarks);
                    roundCompleted = true;
                    break;
                } catch (Exception e) {
                    log.error("iteration {} get page {} raised exception {}"
                            , iteration
                            , page
                            , e.getMessage());
                }
            }

            /**
             *  if round failed - exit cycle without increase page
             *  if finished - exit with finished
             */
            if (!roundCompleted || finished) break;
        }

        log.info("user {} marks collected {}"
                , user
                , marks.size());

        return this;
    }
}
