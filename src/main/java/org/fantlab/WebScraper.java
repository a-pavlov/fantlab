package org.fantlab;

import com.sun.corba.se.spi.ior.MakeImmutable;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.Getter;
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
import java.util.Set;
import java.util.stream.Collectors;

/**
 * Created by apavlov on 08.02.18.
 */
@Slf4j
@Getter
@AllArgsConstructor
public class WebScraper {
    private final WebDriver driver;

    private static final String USER_MARKS_ALL_FORMAT = "%s/markspage%d";



    /**
     * Open the test website.
     */
    public void openFLSite() {
        driver.navigate().to("https://fantlab.ru/");
    }

    public List<String> fetchBooksByGenre(final String genre, int page) {
        /*driver.navigate().to("https://fantlab.ru/bygenre");
        WebElement cb = driver.findElement(By.name(genre));
        if (!cb.isSelected()) cb.click();
        WebElement button = driver.findElement(By.xpath("//form[@name='genrevoteform']/input[@type='submit']"));
        button.click();
        */

        try {
            driver.navigate().to(String.format("https://fantlab.ru/bygenre?form=&lang=&%s=on&page=%d", genre, page));
            List<WebElement> table = driver.findElements(By.xpath("/html/body/div[3]/div/div/div[2]/main/table/tbody/tr"));
            return table.stream().skip(2).map(x -> {
                List<WebElement> hrefs = x.findElements(By.tagName("a"));
                // list no more than one line here
                if (!hrefs.isEmpty() && hrefs.get(0).getAttribute("href").contains("/work")) return hrefs.get(0).getAttribute("href");
                return null;
            }).filter(x  -> x != null).collect(Collectors.toList());
        } catch(Exception e) {
            log.warn("error on fetch books by genre {}", e.getMessage());
        }

        return null;
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
