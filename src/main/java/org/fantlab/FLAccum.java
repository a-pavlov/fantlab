package org.fantlab;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.util.*;

/**
 * Created by apavlov on 11.02.18.
 */
@Getter
@AllArgsConstructor
public class FLAccum {
    /**
     * rows(books) dictionary
     */
    private Map<String, Integer> books;

    /**
     * marks matrix
     */
    private final List<List<Integer>> marks = new LinkedList<List<Integer>>();

    /**
     * users list
     */
    private final List<String> users = new LinkedList<String>();

    private final Map<String, FLUserProfileCollector.UserData> usersData = new HashMap<>();


    /**
     * works in manner - add all marks for user1, next all marks for user2 and so on
     * do not add one mark for user1, user2, ... - incorrect usage here
     * @param userName name of user like userXXXX
     * @param bookName - name of book like workXXXX
     * @param mark - 1-10 mark range
     */
    public void addMark(final String userName, final String bookName, int mark) {
        assert mark != 0;
        if (users.isEmpty() || !users.get(users.size() -1).equals(userName)) {
            // register new user
            users.add(userName);
            marks.add(new ArrayList<Integer>(Collections.nCopies(books.size(), 0)));
        }

        assert marks.size() == users.size();

        Integer index = books.get(bookName);    // try to find that book's index

        if (index == null) {
            // index not found - generate new
            index = books.size();
            books.put(bookName, index);  // create index for book and store it in dictionary

            for(List<Integer> usersMarks: marks) {
                usersMarks.add(0);
            }
        }

        assert index != null;
        assert index < marks.get(marks.size() - 1).size();
        marks.get(marks.size() - 1).set(index, mark);
    }

    public void setUserData(final String userName, final FLUserProfileCollector.UserData userData) {
        usersData.put(userName, userData);
    }
}
