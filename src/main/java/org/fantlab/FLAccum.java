package org.fantlab;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

import java.util.*;
import java.util.stream.Collectors;

/**
 * Created by apavlov on 11.02.18.
 */
@Getter
@Slf4j
public class FLAccum {
    /**
     * rows(books) dictionary
     */
    private Map<String, Integer> books = new HashMap<>();

    /**
     * marks matrix
     */
    private final List<List<Integer>> marks = new ArrayList<>();

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

        int userPos = marks.size() - 1;

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
        assert index < marks.get(userPos).size();
        assert books.size() == marks.get(0).size();
        assert books.size() == marks.get(userPos).size();
        marks.get(userPos).set(index, mark);
        assert !marks.get(userPos).stream().filter(x -> x.compareTo(0) > 0).collect(Collectors.toList()).isEmpty();
    }

    public void setUserData(final String userName, final FLUserProfileCollector.UserData userData) {
        usersData.put(userName, userData);
    }

    public int getTotalMarks() {
        return marks.isEmpty()?0:marks.get(0).size();
    }

    public List<Integer> getMarksByIndex(int index) {
        assert index < getTotalMarks();
        assert !marks.stream().map(x -> x.get(index)).filter(x -> x.compareTo(0) > 0).collect(Collectors.toList()).isEmpty();
        return marks.stream().map(x -> x.get(index)).collect(Collectors.toList());
    }
}
