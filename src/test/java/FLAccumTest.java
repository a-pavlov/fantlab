import org.fantlab.FLAccum;
import org.junit.Test;
import org.junit.internal.matchers.StringContains;

import java.util.HashMap;
import java.util.List;

import static junit.framework.Assert.assertFalse;
import static org.junit.Assert.assertEquals;

/**
 * Created by apavlov on 11.02.18.
 */
public class FLAccumTest {

    @Test
    public void testAccumInitialization() {
        FLAccum accum = new FLAccum();
        accum.addMark("user1", "book1", 10);
        accum.addMark("user1", "book2", 9);
        accum.addMark("user1", "book3", 4);
        assertEquals(3, accum.getBooks().size());
        assertEquals(1, accum.getUsers().size());
        assertEquals(1, accum.getMarks().size());
        assertEquals(3, accum.getMarks().get(0).size());

        accum.addMark("user2", "book1", 3);
        accum.addMark("user2", "book4", 10);
        assertEquals(4, accum.getBooks().size());
        assertEquals(2, accum.getUsers().size());
        assertEquals(2, accum.getMarks().size());
        assertEquals(4, accum.getMarks().get(0).size());
        assertEquals(4, accum.getMarks().get(1).size());

        accum.addMark("user3", "book5", 5);
        assertEquals(5, accum.getBooks().size());
        assertEquals(3, accum.getUsers().size());
        assertEquals(3, accum.getMarks().size());
        assertEquals(5, accum.getMarks().get(0).size());
        assertEquals(5, accum.getMarks().get(1).size());
        assertEquals(5, accum.getMarks().get(2).size());

        assertEquals(0, accum.getMarks().get(0).get(4).intValue());
        assertEquals(0, accum.getMarks().get(1).get(4).intValue());
        assertEquals(5, accum.getMarks().get(2).get(4).intValue());

        for(int i = 1; i < 6; ++i)
            accum.addMark("user4", String.format("book%d", i), i);
        assertEquals(5, accum.getBooks().size());
        assertEquals(4, accum.getUsers().size());
        assertEquals(4, accum.getMarks().size());

        for(int i = 0; i < 4; ++i)
            assertEquals(5, accum.getMarks().get(i).size());
    }

    @Test
    public void testAccumGetters() {
        FLAccum accum = new FLAccum();
        accum.addMark("user1", "book1", 10);
        accum.addMark("user1", "book2", 9);
        accum.addMark("user1", "book3", 4);
        accum.addMark("user2", "book1", 3);
        accum.addMark("user2", "book4", 1);
        accum.addMark("user3", "book5", 8);
        assertEquals(5, accum.getTotalMarks());
        List<Integer> book1 = accum.getMarksByIndex(0);
        assertEquals(3, book1.size());
        assertEquals(10, book1.get(0).intValue());
        assertEquals(3, book1.get(1).intValue());
        assertEquals(0, book1.get(2).intValue());
        List<Integer> book3 = accum.getMarksByIndex(4);
        assertEquals(3, book3.size());
        assertEquals(0, book3.get(0).intValue());
        assertEquals(0, book3.get(1).intValue());
        assertEquals(8, book3.get(2).intValue());

    }

}
