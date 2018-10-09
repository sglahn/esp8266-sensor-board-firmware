FROM python:3 as base

COPY requirements.txt .

RUN pip install -r requirements.txt


FROM python:3.6-alpine

LABEL app.name="mqtt2influx" \
      app.version="1.0" \
      maintainer="Sebastian Glahn <hi@sgla.hn>"

COPY --from=base /root/.cache /root/.cache
COPY --from=base requirements.txt .

RUN pip install -r requirements.txt && rm -rf /root/.cache

COPY mqtt2influx.py /opt/mqtt2influx.py

VOLUME [ "/config" ]

WORKDIR /opt

CMD [ "/opt/mqtt2influx.py", "--config", "/config/mqtt2influx.ini" ]
