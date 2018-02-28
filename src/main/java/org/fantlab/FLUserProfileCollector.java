package org.fantlab;

import lombok.AllArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;

import java.util.List;

/**
 * Created by apavlov on 28.02.18.
 */
@Slf4j
@AllArgsConstructor
public class FLUserProfileCollector {
    private WebDriver driver;
    private String userUrl;


    public void collectUserProfileData() {
        driver.navigate().to(userUrl);
        WebElement table = driver.findElement(By.xpath("/html/body/div[3]/div/div/div[2]/main/div[1]/table/tbody/tr[1]/td[2]/table"));
        List<WebElement> trs = table.findElements(By.tagName("tr"));
        log.info("lines {}", trs.size());
        for(WebElement tr: trs) {
            log.info("tr {}", tr.getText());
        }
    }
}
