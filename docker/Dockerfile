FROM ubuntu:16.04

RUN apt-get update -y
# basic packages

RUN apt-get install -y cmake \
                       sip-dev \
                       libtiff-dev \
                       libfftw3-dev \
                       libeigen3-dev \
                       libpng-dev \
                       python-all \
                       python2.7 \
                       python-qt4 \
                       libboost-all-dev \
                       qt4-qtconfig \
                       qt4-qmake \
                       libqt4-dev \
                       libpng-dev \
                       wget \
                       git \
                       gfortran \
                       libeigen3-dev \
                       python-pip

# Install miniconda - to get useful binaries
RUN wget https://repo.continuum.io/miniconda/Miniconda2-latest-Linux-x86_64.sh
RUN bash Miniconda2-latest-Linux-x86_64.sh -p /opt/miniconda2 -b -f
RUN rm Miniconda2-latest-Linux-x86_64.sh

# Install ClustalW and MSMS
RUN /opt/miniconda2/bin/conda install -y -c bioconda clustalw=2.1 msms=2.6.1

# Install OpenMM
RUN /opt/miniconda2/bin/conda install -y -c omnia openmm=7.1.1=py27_0

# Install DSSP
RUN /opt/miniconda2/bin/conda install -y -c salilab dssp=3.0.0

# Install some python packages
RUN pip install --upgrade pip && pip install numpy==1.14.0 \
                                             scipy==1.0.0 \
                                             pandas==0.22.0 \
                                             jupyter==1.0.0

# copy ost release
WORKDIR /opt/ost

RUN git clone https://git.scicore.unibas.ch/schwede/openstructure.git /opt/ost

# compile ost
RUN cmake . -DPYTHON_LIBRARIES=/usr/lib/x86_64-linux-gnu/libpython2.7.so \
            -DOPTIMIZE=ON \
            -DENABLE_MM=ON \
            -DCOMPILE_TMTOOLS=1 \
            -DUSE_NUMPY=1 \
            -DOPEN_MM_LIBRARY=/opt/miniconda2/pkgs/openmm-7.1.1-py27_0/lib/libOpenMM.so \
            -DOPEN_MM_INCLUDE_DIR=/opt/miniconda2/pkgs/openmm-7.1.1-py27_0/include/ \
            -DOPEN_MM_PLUGIN_DIR=/opt/miniconda2/pkgs/openmm-7.1.1-py27_0/lib/plugins

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
            -DOPEN_MM_LIBRARY=/opt/miniconda2/pkgs/openmm-7.1.1-py27_0/lib/libOpenMM.so \
            -DOPEN_MM_INCLUDE_DIR=/opt/miniconda2/pkgs/openmm-7.1.1-py27_0/include/ \
            -DOPEN_MM_PLUGIN_DIR=/opt/miniconda2/pkgs/openmm-7.1.1-py27_0/lib/plugins \
            -DCOMPOUND_LIB=/opt/ost/compounds.chemlib

# Build OST
RUN make -j4
#RUN make check

# go back home
WORKDIR /home

# Get the example
RUN wget https://files.rcsb.org/view/5X28.pdb

# Set the environment
# - for GUI
ENV QT_X11_NO_MITSHM 1
# - for Python
ENV PYTHONPATH="/opt/ost/stage/lib64/python2.7/site-packages:${PYTHONPATH}"
# - for PATH
ENV PATH="/opt/ost/stage/bin:${PATH}"

# Link binaries
RUN ln -s /opt/miniconda2/pkgs/clustalw*/bin/* /usr/bin
RUN ln -s /opt/miniconda2/pkgs/dssp*/bin/* /usr/bin
RUN ln -s /opt/miniconda2/pkgs/msms*/bin/* /usr/bin

ENTRYPOINT ["ost"]
CMD ["-i"]

