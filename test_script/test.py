import plotly.express as px
import pandas as pd
df = pd.read_csv("data2021.csv", delimiter=";")
latitude = df["lat"].dropna() - 0.02
longitude = df["long"].dropna() * -1
x = pd.DataFrame([latitude, longitude]).transpose()


fig = px.scatter_mapbox(x, 
                        lat="lat", 
                        lon="long", 
                        zoom=8, 
                        height=800,
                        width=800)
fig.update_layout(mapbox_style="open-street-map")
fig.update_layout(margin={"r":0,"t":0,"l":0,"b":0})
fig.show()