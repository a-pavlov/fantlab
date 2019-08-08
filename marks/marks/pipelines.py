# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html

import csv
from marks import settings
import dateutil.parser as dp

class MarksPipeline(object):

    def open_spider(self, spider):
        self.file = open(settings.CSV_FILE_PATH, 'a')
        self.writer = csv.writer(self.file, lineterminator='\n')

    def close_spider(self, spider):
        self.file.close()

    def unix_ts(self, mark):
        if "mark_date_iso" in mark:
            parsed_t = dp.parse(mark['mark_date_iso'])
            return parsed_t.strftime('%s')
        return 0

    def process_item(self, item, spider):
        if "items" in item['json']:
            for mark in item["json"]['items']:
                if "mark" in mark and "user_id" in mark and "work_id" in mark:
                    self.writer.writerow([mark['user_id'], mark['work_id'], mark['mark'], self.unix_ts(mark), 'work_type_id={}'.format(mark.get('work_type_id', 'NA'))])
        return item
