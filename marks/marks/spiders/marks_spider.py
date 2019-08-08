import scrapy
import json
import math

from marks.items import MarksItem
from marks.settings import *


class MarksSpider(scrapy.Spider):
    name = "marks"

    def start_requests(self):
        for userId in range(1, LAST_USER_ID + 1):
            yield scrapy.Request(url='https://api.fantlab.ru/user/{0}/marks'.format(userId), callback=self.parse_first_page)

    def parse_first_page(self, response):
        self.log("incoming url: %s" % response.url)
        json_response = json.loads(response.text)
        if "total_count" in json_response and json_response["total_count"] <= MAX_MARKS_COUNT:            
            for pageNo in range(2, int(math.ceil(json_response["total_count"] / 200.0)) + 1):
                yield scrapy.Request(url="{0}?page={1}".format(response.url, pageNo), callback=self.parse_next_pages)

            res = MarksItem()
            res['json'] = json_response
            yield res
        else:
            self.log('skip {0} due to total marks count {1} > {2}'.format(response.url, json_response.get('total_count'), MAX_MARKS_COUNT))

    def parse_next_pages(self, response):
        res = MarksItem()
        res['json'] = json.loads(response.text)
        yield res