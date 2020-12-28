#! python3
'''#this program scrapes the UChicago Hvz game website and formats the data for time and location of game kills.
this data is then added data for previous games, then analysed and used to create maps of predicted encounters for future games. 
'''
#import modules
import requests, re
import pandas as pd
from bs4 import BeautifulSoup
#create data frame
df = pd.DataFrame([])
#get webpages
for page in range(2043, 2171): #game kills are sequential and do not reset for individual games.
    #these are the actvity pages for game 13
    website = 'https://www.uchicagohvz.org/kill/' + str(page) + '/'
    web = requests.get(website)
    soup = BeautifulSoup(web.content, 'html.parser')
    soup1 = str(soup.p)
    if 'not found' in soup1:#some page numbers are skipped or deleted
        continue
    
    else: 
        tags = []
        #loop gets lat and long values from pages, pages all follow a similar format
        for tag in soup.find_all('script'):
            tags.append(tag)
        latlong = str((tags[7:8]))
        #only includes pages with coordinates
        if 'var' in latlong:
            #format lat and long
            latlong2 = re.split(';|=', latlong)
            lat = float(latlong2[1])
            long = float(latlong2[3])
            #get date, time, day from pages
            tags2 = []
            for tag in soup.find_all('dd'):
                tags2.append(tag)
            dates = str((tags2[-1:]))
            #formats year
            year = re.compile(r'\d\d\d\d')
            year1 = year.search(dates)
            year2 = int(year1.group())
            #formats calander day to day of game
            day = re.compile(r'Oct. (\d)?\d')#month varries by game
            day1 = day.search(dates)
            day2 = str(day1.group())
            day3 = int(day2[5:])-7#number varries by game
            #formats time to 24h
            time = re.compile(r'(\d)?\d:\d\d (A|P)M')
            time1 = time.search(dates)
            time2 = str(time1.group())
            if 'PM' in time2 and '12:' not in time2:
                time3 = (int(time2[:-6])+12)
                time4 = str(time3) + time2[-6:-3]
            else:
                time4 = (time2[:-3])
        else:
            continue 
        #saves data in data frame for each page
        df = df.append(pd.DataFrame({'kill': page, 'lat': lat, 'long': long, 'year': year2, 'day': day3, 'time': time4}, index=[0]), ignore_index=True)
#saves data frame to csv so it can be further analysed and turned into maps
df.to_csv('hvzgame13.csv')
print('done')
