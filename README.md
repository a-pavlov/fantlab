# Recommendation system for fantlab.ru
Collaborative filtering for prediction marks on fantlab.ru based on marks of your co-thinkers.
Math model from Coursera "Machine Learning" by Andrew Ng.

## Parts
* Scraper on Java + Selenium
* Octave code from Coursera's course for collaborative filtering


## Features
* Extract co-thinkers
* Extract genres to narrow down recommendations amount and use only preferred genre
* Extract user's marks, with first your marks
* Learch function for collaborative filtering and provide recommendations

## How to use
* Extract genre using FLMain class with "genre" parameter. Count of pages is hardcoded now 
* Use attached genre(science fiction) books and extract co-thinkers
* Start learning using command octave flrec.m and got recommendations

## Notes
Application by default saves and loads files from some common data directory with octave files. 
In this case octave has access to data files. Do not chage file names, only paths can be changed.

