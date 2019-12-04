FROM atomex/xeus-fift:0.2.2

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

COPY --chown=${NB_USER}:${NB_USER} func_cheat_sheet.ipynb ${HOME}/

WORKDIR ${HOME}
USER ${USER}

ENTRYPOINT []