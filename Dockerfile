FROM atomex/xeus-fift:latest

ARG NB_USER
ARG NB_UID

ENV NB_USER ${NB_USER}
ENV NB_UID ${NB_UID}

USER root
RUN userdel --force jupyter

RUN addgroup ${NB_USER} && adduser -D -G ${NB_USER} -u ${NB_UID} ${NB_USER}
RUN chown -R ${NB_USER} /home/${NB_USER}

RUN pip3 install tlaplus_jupyter
RUN python3 -m tlaplus_jupyter.install

USER ${NB_USER}
WORKDIR /home/${NB_USER}
CMD ["jupyter", "notebook", "--ip", "0.0.0.0"]