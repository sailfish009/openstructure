FROM ubuntu:16.04

# ARGUMENTS
###########

# OBLIGATORY
ARG OPENSTRUCTURE_VERSION
RUN if [ -z "${OPENSTRUCTURE_VERSION}" ]; then \
        echo "OPENSTRUCTURE_VERSION argument is obligatory. Run with --build-arg OPENSTRUCTURE_VERSION=<VERSION>"; \
        exit 1; \
    else : ; fi

# OPTIONAL
ARG SRC_FOLDER="/usr/local/src"
ARG CPUS_FOR_MAKE=8
ARG PYTHONPATH="/usr/local/lib64/python2.7/site-packages:${PYTHONPATH}"
ARG OPENSTRUCTURE_SHARE="/usr/local/share/ost"
ARG MSMS_VERSION="2.6.1"
ARG OPENMM_VERSION="7.1.1"
ARG DSSP_VERSION="2.2.1"
ARG OPENMM_INCLUDE_PATH="/usr/local/openmm/include/"
ARG OPENMM_LIB_PATH="/usr/local/openmm/lib/"

# INSTALL SYSTEM DEPS
#####################
RUN apt-get update -y && apt-get install -y cmake \
                                            sip-dev \
                                            libtiff-dev \
                                            libfftw3-dev \
                                            libeigen3-dev \
                                            libboost-all-dev \
                                            libpng-dev \
                                            python-all \
                                            python2.7 \
                                            python-qt4 \
                                            qt4-qtconfig \
                                            qt4-qmake \
                                            libqt4-dev \
                                            libpng-dev \
                                            wget \
                                            git \
                                            gfortran \
                                            python-pip \
                                            tar \
                                            libbz2-dev \
                                            doxygen \
                                            swig \
                                            clustalw \
                                            python-virtualenv \
                                            locales && \
                      # CLEANUP
                      rm -rf /var/lib/apt/lists/*
# INSTALL SOME PYTHON PACKAGES GLOBALY
######################################
RUN pip install -U pip==9.0.3 && \
    pip install -U setuptools==39.0.1 && \
    pip install --no-cache-dir numpy==1.10.4 \
                         scipy==1.0.0 \
                         pandas==0.22.0

# SET LOCALE
############
# RUN echo "LC_ALL=en_US.UTF-8" >> /etc/environment
# RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen
# RUN echo "LANG=en_US.UTF-8" > /etc/locale.conf
# RUN locale-gen en_US.UTF-8

# DOWNLOAD AND INSTALL MSMS
###########################
RUN cd ${SRC_FOLDER} && \
    mkdir -p msms && \
    cd ${SRC_FOLDER}/msms && \
    wget http://mgltools.scripps.edu/downloads/tars/releases/MSMSRELEASE/REL${MSMS_VERSION}/msms_i86_64Linux2_${MSMS_VERSION}.tar.gz && \
    tar -xvzf msms_i86_64Linux2_${MSMS_VERSION}.tar.gz && \
    cp -v ${SRC_FOLDER}/msms/msms.x86_64Linux2.${MSMS_VERSION} /usr/local/bin/msms && \
    cp -v ${SRC_FOLDER}/msms/pdb_to_xyzr /usr/local/bin/pdb_to_xyzr && \
    cp -v ${SRC_FOLDER}/msms/pdb_to_xyzrn /usr/local/bin/pdb_to_xyzrn && \
    rm -rf ${SRC_FOLDER}/msms

# COMPILE OPENMM FROM SOURCES. INSTALL TO /usr/local
####################################################
RUN cd ${SRC_FOLDER} && \
    wget -O openmm-${OPENMM_VERSION}.tar.gz -nc https://github.com/pandegroup/openmm/archive/${OPENMM_VERSION}.tar.gz && \
    mkdir ${SRC_FOLDER}/openmm-${OPENMM_VERSION} && \
    tar xf openmm-${OPENMM_VERSION}.tar.gz -C ${SRC_FOLDER}/openmm-${OPENMM_VERSION} --strip-components=1 && \
    mkdir -p ${SRC_FOLDER}/openmm-${OPENMM_VERSION}/build && \
    cd ${SRC_FOLDER}/openmm-${OPENMM_VERSION}/build && \
    cmake .. && make -j $CPUS_FOR_MAKE && make install && \
    cd ${SRC_FOLDER}/openmm-${OPENMM_VERSION}/build/python && \
    python setup.py build && python setup.py install && \
    rm -rf ${SRC_FOLDER}/openmm-${OPENMM_VERSION}.tar.gz && \
    rm -rf ${SRC_FOLDER}/openmm-${OPENMM_VERSION}


# COMPILE AND INSTALL DSSP
##########################
RUN cd ${SRC_FOLDER} && \
    wget ftp://ftp.cmbi.umcn.nl/pub/molbio/software/dssp-2/dssp-${DSSP_VERSION}.tgz && \
    tar -xvzf dssp-${DSSP_VERSION}.tgz && \
    cd dssp-${DSSP_VERSION} && \
    make -j ${CPUS_FOR_MAKE} && \
    make install && \
    rm -rf ${SRC_FOLDER}/dssp-${DSSP_VERSION}.tgz && \
    rm -rf ${SRC_FOLDER}/dssp-${DSSP_VERSION}

# INSTALL OST
#############
RUN cd ${SRC_FOLDER} && \
# copy ost release
    wget -O openstructure-${OPENSTRUCTURE_VERSION}.tar.gz -nc https://git.scicore.unibas.ch/schwede/openstructure/repository/${OPENSTRUCTURE_VERSION}/archive.tar.gz && \
    mkdir openstructure-${OPENSTRUCTURE_VERSION} && \
    tar xf openstructure-${OPENSTRUCTURE_VERSION}.tar.gz -C ${SRC_FOLDER}/openstructure-${OPENSTRUCTURE_VERSION} --strip-components=1 && \
    mkdir -p ${SRC_FOLDER}/openstructure-${OPENSTRUCTURE_VERSION}/build
WORKDIR ${SRC_FOLDER}/openstructure-${OPENSTRUCTURE_VERSION}/build

# cmake ost
RUN cmake .. -DPYTHON_LIBRARIES=/usr/lib/x86_64-linux-gnu/libpython2.7.so \
             -DOPTIMIZE=ON \
             -DENABLE_MM=ON \
             -DCOMPILE_TMTOOLS=1 \
             -DUSE_NUMPY=1 \
             -DOPEN_MM_LIBRARY=$OPENMM_LIB_PATH/libOpenMM.so \
             -DOPEN_MM_INCLUDE_DIR=$OPENMM_INCLUDE_PATH \
             -DOPEN_MM_PLUGIN_DIR=$OPENMM_LIB_PATH/plugins \
             -DENABLE_GFX=ON \
             -DENABLE_GUI=ON && \
             # Build chemdict_tool
             make -j ${CPUS_FOR_MAKE} chemdict_tool

# get the compound library
RUN wget ftp://ftp.wwpdb.org/pub/pdb/data/monomers/components.cif.gz
RUN stage/bin/chemdict_tool create components.cif.gz compounds.chemlib pdb && stage/bin/chemdict_tool update ../modules/conop/data/charmm.cif compounds.chemlib charmm
RUN mkdir -p $OPENSTRUCTURE_SHARE && chmod a+rw -R $OPENSTRUCTURE_SHARE && mv compounds.chemlib $OPENSTRUCTURE_SHARE

# Build and install OST
RUN cmake .. -DPYTHON_LIBRARIES=/usr/lib/x86_64-linux-gnu/libpython2.7.so \
             -DOPTIMIZE=ON \
             -DENABLE_MM=ON \
             -DCOMPILE_TMTOOLS=1 \
             -DUSE_NUMPY=1 \
             -DOPEN_MM_LIBRARY=$OPENMM_LIB_PATH/libOpenMM.so \
             -DOPEN_MM_INCLUDE_DIR=$OPENMM_INCLUDE_PATH \
             -DOPEN_MM_PLUGIN_DIR=$OPENMM_LIB_PATH/plugins \
             -DENABLE_GFX=ON \
             -DENABLE_GUI=ON \
             -DCOMPOUND_LIB=$OPENSTRUCTURE_SHARE/compounds.chemlib && \
             # Build OST with compound library
             make -j ${CPUS_FOR_MAKE} && make check && make install

# GO HOME AND CLEANUP
#####################
# RUN apt-get purge -y cmake \
#                      wget \
#                      git \
#                      gfortran \
#                      python-pip \
#                      libbz2-dev \
#                      doxygen \
#                      swig
# RUN apt-get clean
# RUN apt-get autoremove -y
# 
# WORKDIR $SRC_FOLDER
# RUN rm -rf $SRC_FOLDER/*

WORKDIR /home

# ENVIRONMENT
##############################################################################
ENV OST_ROOT="/usr/local"
ENV PYTHONPATH="/usr/local/lib64/python2.7/site-packages:${PYTHONPATH}"
ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib64"
ENV QT_X11_NO_MITSHM=1

ENTRYPOINT ["ost"]
CMD ["-i"]
