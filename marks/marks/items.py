# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# https://docs.scrapy.org/en/latest/topics/items.html

import scrapy


class MarksItem(scrapy.Item):
    json = scrapy.Field()

    def __repr__(self):
        """only print out attr1 after exiting the Pipeline"""
        #return repr({"attr1": self.attr1})
        return
