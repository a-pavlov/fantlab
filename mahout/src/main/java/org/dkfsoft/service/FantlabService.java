package org.dkfsoft.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import okhttp3.ConnectionPool;
import okhttp3.OkHttpClient;
import okhttp3.logging.HttpLoggingInterceptor;
import org.dkfsoft.client.FantlabApiClient;
import org.dkfsoft.model.UserId;
import org.dkfsoft.model.Work;
import retrofit2.Retrofit;
import retrofit2.adapter.rxjava.RxJavaCallAdapterFactory;
import retrofit2.converter.jackson.JacksonConverterFactory;
import retrofit2.converter.scalars.ScalarsConverterFactory;
import rx.Observable;
import rx.schedulers.Schedulers;

import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;

import static okhttp3.logging.HttpLoggingInterceptor.Level.BODY;
import static okhttp3.logging.HttpLoggingInterceptor.Level.NONE;


public class FantlabService {
    private final FantlabApiClient fantlabApiClient;
    private final ExecutorService executorService;

    public FantlabService(final ObjectMapper objectMapper, final ExecutorService executorService) {
        this.fantlabApiClient = new Retrofit.Builder()
                .baseUrl("https://api.fantlab.ru/")
                .addCallAdapterFactory(RxJavaCallAdapterFactory.create())
                .addConverterFactory(ScalarsConverterFactory.create())
                .addConverterFactory(JacksonConverterFactory.create(objectMapper))
                .client(new OkHttpClient.Builder()
                        .addInterceptor(new HttpLoggingInterceptor().setLevel(NONE))
                        .connectionPool(new ConnectionPool(0,10, TimeUnit.SECONDS)).build())
                .build().create(FantlabApiClient.class);
        this.executorService = executorService;
    }

    public List<Work> getWorkDetails(final List<Long> workIds) {
        return Observable.from(workIds)
                .flatMap(x -> Observable.just(x)
                        .subscribeOn(Schedulers.from(executorService))
                        .flatMap(item -> fantlabApiClient.getWork("work/" + Long.toString(item) + "/extended")
                                .onErrorReturn((e) -> null)))
                .toList()
                .toBlocking()
                .first()
                .stream()
                .filter(x -> x != null && x.isSuccessful())
                .map(x -> x.body())
                .collect(Collectors.toList());
    }

    public UserId getUserIdByLogin(final String login) {
        return fantlabApiClient.getUserByLogin("userlogin", login)
                //.retryWhen(errors -> errors.flatMap(error -> Observable.just(null)))
                .toBlocking().first().body();
    }
}
