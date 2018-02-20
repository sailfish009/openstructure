FROM ubuntu:16.04

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
                                            libeigen3-dev \
                                            python-pip \
                                            tar \
                                            libbz2-dev \
                                            doxygen \
                                            swig \
                                            clustalw


# Install MSMS
WORKDIR /
RUN mkdir -p /msms
WORKDIR /msms
RUN wget http://mgltools.scripps.edu/downloads/tars/releases/MSMSRELEASE/REL2.6.1/msms_i86_64Linux2_2.6.1.tar.gz
RUN tar -xvzf msms_i86_64Linux2_2.6.1.tar.gz
RUN ln -s /msms/msms.x86_64Linux2.2.6.1 /usr/bin/msms
RUN ln -s /msms/pdb_to_xyzr /usr/bin/pdb_to_xyzr
RUN ln -s /msms/pdb_to_xyzrn /usr/bin/pdb_to_xyzrn


# Install OpenMM
WORKDIR /
RUN git clone https://github.com/pandegroup/openmm
WORKDIR /openmm
RUN git checkout -b 7.1.1 tags/7.1.1
RUN cmake .
RUN make -j4
RUN make install

# Install DSSP
WORKDIR /
RUN wget ftp://ftp.cmbi.ru.nl/pub/software/dssp/dssp-2.2.1.tgz
RUN tar -xvzf dssp-2.2.1.tgz
WORKDIR /dssp-2.2.1
RUN make
RUN ln -s /dssp-2.2.1/mkdssp /usr/bin/mkdssp

# Install some python packages
WORKDIR /
RUN pip install --upgrade pip && pip install numpy==1.10.4 \
                                             scipy==1.0.0 \
                                             pandas==0.22.0 \
                                             jupyter==1.0.0

# copy ost release
WORKDIR /opt/ost

RUN git clone https://git.scicore.unibas.ch/schwede/openstructure.git /opt/ost && git checkout -b develop origin/develop

# compile ost
RUN cmake . -DPYTHON_LIBRARIES=/usr/lib/x86_64-linux-gnu/libpython2.7.so \
            -DOPTIMIZE=ON \
            -DENABLE_MM=ON \
            -DCOMPILE_TMTOOLS=1 \
            -DUSE_NUMPY=1 \
            -DOPEN_MM_LIBRARY=/usr/local/openmm/lib/libOpenMM.so \
            -DOPEN_MM_INCLUDE_DIR=/usr/local/openmm/include \
            -DOPEN_MM_PLUGIN_DIR=/usr/local/openmm/lib/plugins

# Build chemdict_tool
RUN make -j4 chemdict_tool

# get the compound library
RUN wget ftp://ftp.wwpdb.org/pub/pdb/data/monomers/components.cif.gz
RUN stage/bin/chemdict_tool create components.cif.gz compounds.chemlib pdb
RUN stage/bin/chemdict_tool update modules/conop/data/charmm.cif compounds.chemlib charmm

# Reconfigure OST
RUN cmake . -DPYTHON_LIBRARIES=/usr/lib/x86_64-linux-gnu/libpython2.7.so \
            -DOPTIMIZE=ON \
            -DENABLE_MM=ON \
            -DOPEN_MM_LIBRARY=/usr/local/openmm/lib/libOpenMM.so \
            -DOPEN_MM_INCLUDE_DIR=/usr/local/openmm/include \
            -DOPEN_MM_PLUGIN_DIR=/usr/local/openmm/lib/plugins \
            -DCOMPOUND_LIB=/opt/ost/compounds.chemlib

# Build OST
RUN make -j4
RUN make check

# go back home
WORKDIR /home

# Set the environment
# - for GUI
ENV QT_X11_NO_MITSHM 1
# - for Python
ENV PYTHONPATH="/opt/ost/stage/lib64/python2.7/site-packages:${PYTHONPATH}"
# - for PATH
ENV PATH="/opt/ost/stage/bin:${PATH}"

ENTRYPOINT ["ost"]
CMD ["-i"]

