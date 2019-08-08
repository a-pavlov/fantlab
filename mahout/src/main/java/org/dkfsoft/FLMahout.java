package org.dkfsoft;

import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import okhttp3.ConnectionPool;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.logging.HttpLoggingInterceptor;
import org.apache.mahout.cf.taste.common.TasteException;
import org.apache.mahout.cf.taste.impl.model.file.FileDataModel;
import org.apache.mahout.cf.taste.impl.neighborhood.NearestNUserNeighborhood;
import org.apache.mahout.cf.taste.impl.recommender.GenericUserBasedRecommender;
import org.apache.mahout.cf.taste.impl.similarity.PearsonCorrelationSimilarity;
import org.apache.mahout.cf.taste.neighborhood.UserNeighborhood;
import org.apache.mahout.cf.taste.recommender.RecommendedItem;
import org.apache.mahout.cf.taste.recommender.Recommender;
import org.apache.mahout.cf.taste.similarity.UserSimilarity;
import org.dkfsoft.client.FantlabApiClient;
import org.dkfsoft.model.Work;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.adapter.rxjava.RxJavaCallAdapterFactory;
import retrofit2.converter.jackson.JacksonConverterFactory;
import retrofit2.converter.scalars.ScalarsConverterFactory;

import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.concurrent.TimeUnit;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static okhttp3.logging.HttpLoggingInterceptor.Level.BODY;

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

        ObjectMapper objectMapper = new ObjectMapper();
        objectMapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        objectMapper.setSerializationInclusion(NON_NULL);

        FantlabApiClient fantlabApiClient = new Retrofit.Builder()
                .baseUrl("https://api.fantlab.ru/")
                .addCallAdapterFactory(RxJavaCallAdapterFactory.create())
                .addConverterFactory(ScalarsConverterFactory.create())
                .addConverterFactory(JacksonConverterFactory.create(objectMapper))
                .client(new OkHttpClient.Builder()
                        .addInterceptor(new HttpLoggingInterceptor().setLevel(BODY))
                        .connectionPool(new ConnectionPool(0,10,TimeUnit.SECONDS)).build())
                        .build().create(FantlabApiClient.class);

        Response<Work> work = fantlabApiClient.getWork("work/1958/extended").toBlocking().first();


        FileDataModel dataModel = new FileDataModel(new File(args[0]));
        UserSimilarity userSimilarity = new PearsonCorrelationSimilarity(dataModel);

        UserNeighborhood neighborhood = new NearestNUserNeighborhood(100, userSimilarity, dataModel);
        Recommender recommender = new GenericUserBasedRecommender(dataModel, neighborhood, userSimilarity);
        //User user = dataModel.getUser(userId);
        //System.out.println("-----");
        //System.out.println("User: " + user);
        //org.dkfsoft.TasteUtils.printPreferences(user, handler.map);
        List<RecommendedItem> recommendations =
                recommender.recommend(userId, 50);
        TasteUtils.printRecs(recommendations, null);
    }
}
