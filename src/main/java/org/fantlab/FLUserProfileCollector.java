package org.fantlab;

import lombok.*;
import lombok.extern.slf4j.Slf4j;
import okhttp3.internal.Internal;
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;

import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by apavlov on 28.02.18.
 */
@Slf4j
@Getter
public class FLUserProfileCollector extends FLCollector {
    private final String userUrl;

    public static Pattern regPattern = Pattern.compile("^Оценок\\s+(?<d1>[0-9-:\\s]+)");
    public static Pattern enterPattern = Pattern.compile("^Оценок\\s+(?<d1>[0-9-:\\s]+)");
    public static Pattern markPattern = Pattern.compile("^Оценок\\s+(?<d1>\\d+)");
    public static Pattern recallPattern = Pattern.compile("^Отзывов\\s*(?<d1>\\d+)(\\s*\\|\\s+баллы\\s+за\\s+отзывы:\\s*)*(?<d2>\\d+)*");
    public static Pattern annotationPattern = Pattern.compile("^Аннотаций\\s+(?<d1>\\d+)");
    public static Pattern requestPatern = Pattern.compile("^Заявок\\s+(?<d1>\\d+)");
    public static Pattern classPattern = Pattern.compile("^Классифицировано\\s+произведений\\s+(?<d1>\\d+)");
    public static Pattern fmsgPattern = Pattern.compile("^Сообщений\\s+в\\s+форуме\\s+(?<d1>\\d+)");
    public static Pattern newThemesPattern = Pattern.compile("^Новых\\s+тем\\s+в\\s+форуме\\s+(?<d1>\\d+)");
    public static Pattern surveyPattern = Pattern.compile("^Новых\\s+опросов\\s+в\\s+форуме\\s+(?<d1>\\d+)");
    public static Pattern shelfPattern = Pattern.compile("^Личных\\s+книжных\\s+полок\\s+(?<d1>\\d+)");


    @Data
    @EqualsAndHashCode
    @AllArgsConstructor
    @NoArgsConstructor
    @ToString
    public static class UserData {
        private int marks       = 0;
        private int recalls     = 0;
        private int recallPoints= 0;
        private int annotations = 0;
        private int requests    = 0;
        private int classes     = 0;
        private int forumMessages= 0;
        private int newThemes   = 0;
        private int surveys     = 0;
        private int shelves     = 0;
    }

    UserData userData = new UserData();

    public FLUserProfileCollector(final WebDriver driver, final String userUrl) {
        this.driver = driver;
        this.userUrl = userUrl;
    }

    public void extractDataFromString(final String s) {
        Matcher m;
        if ((m = markPattern.matcher(s)).find()) {
            userData.setMarks(Integer.parseInt(m.group("d1")));
        }
        else if ((m = recallPattern.matcher(s)).find()) {
            userData.setRecalls(Integer.parseInt(m.group("d1")));
            if (m.group("d2") != null) {
                userData.setRecallPoints(Integer.parseInt(m.group("d2")));
            }
        }
        else if ((m = annotationPattern.matcher(s)).find()) {
            userData.setAnnotations(Integer.parseInt(m.group("d1")));
        }
        else if ((m = requestPatern.matcher(s)).find()) {
            userData.setRequests(Integer.parseInt(m.group("d1")));
        }
        else if ((m = classPattern.matcher(s)).find()) {
            userData.setClasses(Integer.parseInt(m.group("d1")));
        }
        else if ((m = fmsgPattern.matcher(s)).find()) {
            userData.setForumMessages(Integer.parseInt(m.group("d1")));
        }
        else if ((m = newThemesPattern.matcher(s)).find()) {
            userData.setNewThemes(Integer.parseInt(m.group("d1")));
        }
        else if ((m = surveyPattern.matcher(s)).find()) {
            userData.setSurveys(Integer.parseInt(m.group("d1")));
        }
        else if ((m = shelfPattern.matcher(s)).find()) {
            userData.setShelves(Integer.parseInt(m.group("d1")));
        }
    }

    @Override
    public void collect() {
        try {
            driver.navigate().to(userUrl);
            if (driver.getTitle().equals("503 Service Temporarily Unavailable")) {
                log.warn("service temporary unavailable, exit");
                return;
            }

            WebElement table = driver.findElement(By.xpath("/html/body/div[3]/div/div/div[2]/main/div[1]/table/tbody/tr[1]/td[2]/table"));
            List<WebElement> trs = table.findElements(By.tagName("tr"));
            log.info("lines {}", trs.size());
            for (WebElement tr : trs) {
                extractDataFromString(tr.getText());
            }

            finished = true;
        } catch (Exception e) {
            log.warn("get user profile error {}", e.getMessage());
        }
    }

    @Override
    public void storeResuls(FLAccum accum) {
        assert isFinished();
        accum.setUserData(userUrl, userData);
    }
}
