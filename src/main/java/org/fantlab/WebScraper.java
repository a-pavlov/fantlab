package org.fantlab;

import com.sun.corba.se.spi.ior.MakeImmutable;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.ToString;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.io.FileUtils;
import org.openqa.selenium.*;
import org.openqa.selenium.firefox.FirefoxDriver;
import org.openqa.selenium.interactions.Interaction;

import java.io.*;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/**
 * Created by apavlov on 08.02.18.
 */
@Slf4j
public class WebScraper {
    public WebDriver driver = new FirefoxDriver();

    private static final String USER_MARKS_ALL_FORMAT = "%s/marks%s";

    @Data
    @AllArgsConstructor
    @ToString
    public static class Mark {
        public String url;
        public int value;
    }

    /**
     * Open the test website.
     */
    public void openFLSite() {
        driver.navigate().to("https://fantlab.ru/");
    }

    public List<WebElement> openHLinks() {
        driver.navigate().to("https://fantlab.ru/hlinks"); //?all=1");
        //List<WebElement> l1 = driver.findElements(By.xpath("/html/body/div[3]/div/div/div[2]/main/table[2]/tbody/tr/td[1]/table/tbody/tr[4]/td/table/tbody/tr[1]/td[2]/b/a"));
        List<WebElement> column = driver.findElements(By.xpath("/html/body/div[3]/div/div/div[2]/main/table[2]/tbody/tr/td[1]/table/tbody/tr"));
        log.info("column size {}", column.size());
        List<WebElement> elements = new LinkedList<WebElement>();
        for(final WebElement cell: column) {
            List<WebElement> links = cell.findElements(By.tagName("a"));
            elements.addAll(links);
        }

        return elements;
    }

    /**
     *
     * @param username
     * @param password
     *
     *            Logins into the website, by entering provided username and
     *            password
     */
    public void login(String username, String password) {

        WebElement userName_editbox = driver.findElement(By.name("login"));
        WebElement password_editbox = driver.findElement(By.name("password"));
        WebElement submit_button = driver.findElement(By.xpath("//button[contains(text(),'OK')]"));

        userName_editbox.sendKeys(username);
        password_editbox.sendKeys(password);
        submit_button.click();
    }

    public List<Mark> getUserMarks(final String user) {
        assert user != null;
        List<Mark> marks = new LinkedList<Mark>();
        for(int page = 0; page < 2; ++page) {
            String pageUrl = String.format(USER_MARKS_ALL_FORMAT, user, (page==0)?"":"page" + Integer.toString(page));
            driver.navigate().to(pageUrl);
            List<WebElement> table = driver.findElements(By.xpath("/html/body/div[3]/div/div/div[2]/main/table[1]/tbody/tr"));

            log.info("navigate to {} table size {}"
                    , pageUrl
                    , table.size());

            if (table.isEmpty()) break;

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
                    // just ignore
                }

                if (mark > 0) {
                    WebElement tda = tds.get(0).findElement(By.tagName("a"));
                    if (tda != null) {
                        marks.add(new Mark(tda.getAttribute("href"), mark));
                    }
                }

            }

            log.info("total marks {}", marks.size());
        }

        return marks;
    }

    public void getText() throws IOException {
        //String text = driver.findElement(By.xpath("//div[@id='case_login']/h3")).getText();
        //Writer writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream("status.txt"), "utf-8"));
        //writer.write(text);
        //writer.close();
    }

    /**
     * Saves the screenshot
     *
     * @throws IOException
     */
    public void saveScreenshot() throws IOException {
        File scrFile = ((TakesScreenshot) driver).getScreenshotAs(OutputType.FILE);
        FileUtils.copyFile(scrFile, new File("screenshot.png"));
    }

    public void closeBrowser() {
        driver.close();
    }
}
