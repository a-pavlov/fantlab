package org.fantlab;

import lombok.extern.slf4j.Slf4j;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by apavlov on 10.03.18.
 */
@Slf4j
public class FLMahout {

    private static final Pattern ptrUser = Pattern.compile("^user(?<num>\\d+)$");
    private static final Pattern ptrBook = Pattern.compile("^work(?<num>\\d+)$");
    private static final Pattern ptrZero = Pattern.compile("^0+$");

    /**
     * extract users/works from sorted! by positions csv files
     * @param filename csv file format num,id
     * @param pattern appropriate pattern line user/work
     * @param sink container
     * @throws FLException
     */
    private static void loadCsvTrivial(final String filename
            , final Pattern pattern
            , final List<String> sink) throws FLException {
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            String line;
            int lineNumber = 0;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split(",");

                if (parts.length != 2) {
                    throw new FLException("Incorrect format of file " + filename + " line " + lineNumber);
                }

                Matcher mt = pattern.matcher(parts[1]);

                if (!mt.find()) {
                    throw new FLException("Match error in " + filename + " line " + lineNumber);
                }

                String res = mt.group("num");
                assert res != null;
                sink.add(res);
                ++lineNumber;
            }
        } catch(IOException e) {
            throw new FLException("I/O error " + e.getMessage());
        }
    }

    public static void main(String args[]) {
        try(InputStream input = FLMain.class.getClassLoader().getResourceAsStream("config.properties")) {
            Properties prop = new Properties();
            prop.load(input);
            final String filenameBooks = prop.getProperty("books_file");
            final String filenameUsers = prop.getProperty("users_file");
            final String filenameMarks = prop.getProperty("marks_file");
            final String filenameMahout = prop.getProperty("mahout_file");

            if (filenameBooks == null
                    || filenameMarks == null
                    || filenameUsers == null
                    || filenameMahout == null) throw new FLException("books_file/users_file/marks_file/mahout_file are missed");


            List<String> users = new ArrayList<>();
            List<String> books = new ArrayList<>();

            loadCsvTrivial(filenameUsers, ptrUser, users);
            loadCsvTrivial(filenameBooks, ptrBook, books);

            log.info("users {} books {}", users.size(), books.size());

            try(BufferedReader br = new BufferedReader(new FileReader(filenameMarks)); PrintWriter writer = new PrintWriter(filenameMahout)) {
                String line;
                int bookNo = 0;
                while ((line = br.readLine()) != null) {
                    String[] userMarks = line.split(",");

                    if (userMarks.length != users.size()) {
                        throw new FLException("Incorrect users on line " + bookNo);
                    }

                    if (bookNo >= books.size()) {
                        throw new FLException("Book index out of bound books dictionary");
                    }

                    for(int i = 0; i < userMarks.length; ++i) {
                        if (!ptrZero.matcher(userMarks[i]).find()) {
                            StringBuilder sb = new StringBuilder();
                            sb.append(users.get(i))
                                    .append(",")
                                    .append(books.get(bookNo))
                                    .append(",")
                                    .append(userMarks[i])
                                    .append("\n");
                            writer.write(sb.toString());
                        }
                    }

                    ++bookNo;

                }
            } catch (IOException e) {
                log.error("I/O exception {}", e.getMessage());
                throw new FLException("I/O exception on reading marks and writing mahout");
            }

        } catch(FLException e) {
            log.error("FL error {}", e.getMessage());
        } catch(IOException e ){
            log.error("unable to open properties file {}", e.getMessage());
        }
    }
}
