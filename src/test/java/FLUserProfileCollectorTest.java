import lombok.extern.slf4j.Slf4j;
import org.fantlab.FLUserProfileCollector;
import org.junit.Test;

import java.util.regex.Matcher;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

/**
 * Created by apavlov on 01.03.18.
 */
@Slf4j
public class FLUserProfileCollectorTest {

    @Test
    public void testMarksPattern() {
        Matcher m = FLUserProfileCollector.markPattern.matcher("Оценок 8");
        assertTrue(m.find());
        assertEquals("8", m.group("d1"));
    }

    @Test
    public void testRecalls() {
        Matcher m1 = FLUserProfileCollector.recallPattern.matcher("Отзывов 94  |  баллы за отзывы: 635");
        Matcher m2 = FLUserProfileCollector.recallPattern.matcher("Отзывов 33");
        assertTrue(m1.find());
        assertTrue(m2.find());
        assertEquals("94", m1.group("d1"));
        assertEquals("635", m1.group("d2"));
        assertEquals("33", m2.group("d1"));
        assertEquals(null, m2.group("d2"));

        Matcher m3 = FLUserProfileCollector.recallPattern.matcher("Личных книжных полок 8");
        assertFalse(m3.find());
    }

    @Test
    public void testAnnotationPattern() {
        Matcher m = FLUserProfileCollector.annotationPattern.matcher("Аннотаций 9098");
        assertTrue(m.find());
        assertEquals("9098", m.group("d1"));
    }

    @Test
    public void testClassPattern() {
        Matcher m = FLUserProfileCollector.classPattern.matcher("Классифицировано произведений   105");
        assertTrue(m.find());
        assertEquals("105", m.group("d1"));
    }

    @Test
    public void testMsgPattern() {
        Matcher m = FLUserProfileCollector.fmsgPattern.matcher("Сообщений в форуме 7881");
        assertTrue(m.find());
        assertEquals("7881", m.group("d1"));
    }

    @Test
    public void testNewThemesPattern() {
        Matcher m = FLUserProfileCollector.newThemesPattern.matcher("Новых тем в форуме 249");
        assertTrue(m.find());
        assertEquals("249", m.group("d1"));
    }

    @Test
    public void testSurveyPattern() {
        Matcher m = FLUserProfileCollector.surveyPattern.matcher("Новых опросов в форуме 13");
        assertTrue(m.find());
        assertEquals("13", m.group("d1"));
    }

    @Test
    public void testShelfPatterns() {
        Matcher m = FLUserProfileCollector.shelfPattern.matcher("Личных книжных полок 8");
        assertTrue(m.find());
        assertEquals("8", m.group("d1"));
    }

    @Test
    public void testDataExtraction() {
        FLUserProfileCollector collector = new FLUserProfileCollector(null, null);
        collector.extractDataFromString("");

        collector.extractDataFromString("Пол мужской");
        collector.extractDataFromString("День рождения 6 мая 1981 г.");
        collector.extractDataFromString("Место жительства Россия, Самара");

        collector.extractDataFromString("Skype crealist");

        collector.extractDataFromString("Домашняя страница http://fantlab.ru");

        collector.extractDataFromString("Дата регистрации 2004-07-17 22:11:01");

        collector.extractDataFromString("Последнее посещение 2018-03-01 07:59:12");

        collector.extractDataFromString("Класс философ");

        collector.extractDataFromString("Развитие в классе 3868.54 из 4000");

        collector.extractDataFromString("Оценок 313");

        collector.extractDataFromString("Отзывов 94  |  баллы за отзывы: 635");

        collector.extractDataFromString("Аннотаций 118");

        collector.extractDataFromString("Классифицировано произведений   105");

        collector.extractDataFromString("Заявок 68");

        collector.extractDataFromString("Сообщений в форуме 7881");

        collector.extractDataFromString("Новых тем в форуме 249");

        collector.extractDataFromString("Новых опросов в форуме 13");

        collector.extractDataFromString("Личных книжных полок 8");

        assertEquals(new FLUserProfileCollector.UserData(313, 94, 635, 118, 68, 105, 7881, 249, 13, 8), collector.getUserData());

    }
}
