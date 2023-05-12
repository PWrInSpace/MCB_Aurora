# import gmplot package
import gmplot
import pandas as pd
df = pd.read_csv("Flight_R4D_1666000178.csv", delimiter=";")
latitude = df["lat"].dropna()
longitude = df["long"].dropna()
# x = pd.DataFrame([latitude, longitude]).transpose()

# latitude_list = [ 30.3358376, 30.307977, 30.3216419 ]
# longitude_list = [ 77.8701919, 78.048457, 78.0413095 ]
  
gmap3 = gmplot.GoogleMapPlotter(latitude[0],
                                longitude[1], 25,  map_type='satellite')
  
# scatter method of map object 
# scatter points on the google map
gmap3.scatter( latitude, longitude, '# FF0000',
                              size = 40, marker = False )
  
# Plot method Draw a line in
# between given coordinates
gmap3.plot(latitude, longitude, 
           'cornflowerblue', edge_width = 2.5)
  
gmap3.draw( "test.html" )