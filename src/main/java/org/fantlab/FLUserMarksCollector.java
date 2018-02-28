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
import java.util.Set;

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

    private final String user;
    private final int maxMarks;
    private final Set<String> genre;

    /**
     * web driver as is
     */
    private WebDriver driver;

    /**
     *  current page
     */
    int page = 1;

    private boolean finished  = false;


    public FLUserMarksCollector(final WebDriver driver
            , final String user
            , int maxMarks
            , final Set<String> genre) {
        this.driver = driver;
        this.user = user;
        this.maxMarks = maxMarks;
        this.genre = genre;
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
    private int getUserMarksFromPage(final String pageUrl, List<Mark> marks, int maxMarks) {
        driver.navigate().to(pageUrl);
        List<WebElement> table = driver.findElements(By.xpath("/html/body/div[3]/div/div/div[2]/main/table[1]/tbody/tr"));

        log.debug("navigate to {} table size {}", pageUrl, table.size());

        List<Mark> localMarks = new LinkedList<>();
        int marksOnPage = 0;

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
               log.trace("hmm mark is not integer");
            }

            if (mark > 0) {
                WebElement tda = tds.get(0).findElement(By.tagName("a"));
                if (tda != null) {
                    if (genre.isEmpty() || genre.contains(FLUtil.link2Name(tda.getAttribute("href"))))
                    {
                        localMarks.add(new Mark(tda.getAttribute("href"), mark));
                    }

                    ++marksOnPage;
                }
            }

            if (marks.size() + localMarks.size() >= maxMarks) break;
        }

        marks.addAll(localMarks);
        return marksOnPage;
    }

    public FLUserMarksCollector collectUserMarks() {
        assert user != null;

        // do not check users links here
        /*try {
            if (totalMarkPages == -1) {
                String pageUrl = String.format(USER_MARKS_ALL_FORMAT, user, 1);
                driver.navigate().to(pageUrl);

                WebElement div = driver.findElement(By.xpath("/html/body/div[3]/div/div/div[2]/main/div[3]"));

                if (div == null) {
                    collectorStatus = CollectorStatus.CS_BAD;
                    log.warn("didn't find div element with links");
                } else {
                    List<WebElement> pageLinks = div.findElements(By.tagName("a"));

                    if (pageLinks.size() > pagesPerUserLimit) {
                        log.warn("user {} has pages {} greater than limit {}", user, pageLinks.size(), pagesPerUserLimit);
                        collectorStatus = CollectorStatus.CS_BAD;
                    }

                    totalMarkPages = pageLinks.size();
                }
            }
        } catch(Exception e) {
            log.warn("total pages count for user error {}", e.getMessage());
            totalMarkPages = 0;
        }
        */

        for (; page < MAX_PAGES; ++page) {
            String pageUrl = String.format(USER_MARKS_ALL_FORMAT, user, page);

            try {
                int marksOnPage = getUserMarksFromPage(pageUrl, marks, maxMarks);
                finished = (marksOnPage == 0 || marks.size() >= maxMarks);
            } catch (Exception e) {
                log.warn("get page {} raised exception {}", page, e.getMessage());
            }

            /**
             *  if round failed - exit cycle without increase page
             *  if finished - exit with finished
             */
            if (finished) break;
        }

        log.info("user {} marks collected {}", user, marks.size());
        return this;
    }
}
