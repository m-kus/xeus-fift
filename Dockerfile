FROM atomex/xeus-fift:0.2.1

ARG NB_USER
ARG NB_UID

USER root

ENV USER ${NB_USER}
ENV HOME /home/${NB_USER}

RUN userdel --force jupyter
RUN adduser --disabled-password \
    --gecos "Default user" \
    --uid ${NB_UID} \
    ${NB_USER}

WORKDIR ${HOME}
USER ${USER}

ENTRYPOINT []