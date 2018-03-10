import org.apache.mahout.cf.taste.common.TasteException;
import org.apache.mahout.cf.taste.impl.model.file.FileDataModel;
import org.apache.mahout.cf.taste.impl.neighborhood.NearestNUserNeighborhood;
import org.apache.mahout.cf.taste.impl.recommender.GenericUserBasedRecommender;
import org.apache.mahout.cf.taste.impl.similarity.PearsonCorrelationSimilarity;
import org.apache.mahout.cf.taste.neighborhood.UserNeighborhood;
import org.apache.mahout.cf.taste.recommender.RecommendedItem;
import org.apache.mahout.cf.taste.recommender.Recommender;
import org.apache.mahout.cf.taste.similarity.UserSimilarity;

import java.io.File;
import java.io.IOException;
import java.util.List;

/**
 * Created by apavlov on 10.03.18.
 */
public class FLMahout {

    public static void main(String args[]) throws IOException, TasteException {

        if (args.length != 2) {
            System.out.print("Please provide filename with data and user id for recommendations");
            return;
        }

        int userId = Integer.parseInt(args[1]);

        System.out.println("-----");
        System.out.println("started from " + args[0] + " for user " + args[1]);
        System.out.println("-----");

        FileDataModel dataModel = new FileDataModel(new File(args[0]));
        UserSimilarity userSimilarity = new PearsonCorrelationSimilarity(dataModel);

        UserNeighborhood neighborhood = new NearestNUserNeighborhood(100, userSimilarity, dataModel);
        Recommender recommender = new GenericUserBasedRecommender(dataModel, neighborhood, userSimilarity);
        //User user = dataModel.getUser(userId);
        //System.out.println("-----");
        //System.out.println("User: " + user);
        //TasteUtils.printPreferences(user, handler.map);
        List<RecommendedItem> recommendations =
                recommender.recommend(userId, 5);
        TasteUtils.printRecs(recommendations, null);
    }
}
