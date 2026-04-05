# How to create conda environment 

This document describes the steps to create conda environment for lecture 10 as well as the final exam. First you need to install anaconda on your computer. I recommend miniconda, you can download from 

https://www.anaconda.com/download/success 

You may need to run 3 command lines that appear after you try the first command below.

## Step 1 

Create environment and activate it. You can use any environment name in place of iaiiot

```console
conda create -n iaiiot
conda activate iaiiot
```

## Step 2 

Install specific version of Python. Since you need to install gymnasium later on, it requires Python<=3.13 

```console
conda install python=3.13
```

**Note :** 

1. If the above command does not work, try

```console
conda install -c conda-forge python=3.13
```

2. Step 1-2 can be combined as

```console
conda create -n iaiiot python=3.13
conda activate iaiiot
```

## Step 3

Now we are in the iaiiot environment (or whatever name you use from step 1). To use jupyter notebook, install jupyterlab

```console
conda install conda-forge::jupyterlab
```

## Step 4

Install any package you want to use. For example,

```console
pip install numpy matplotlib paho-mqtt torch torchvision torchaudio
```

To install Gymnasium (useful for final exam)

For Windows

```console
pip install gymnasium
pip install gymnasium[classic-control]
pip install gymnasium[mujoco]
pip install swig
pip install gymnasium[box2d]
```

For Mac-OSX, zsh requires slightly different format

```console
pip install gymnasium
pip install 'gymnasium[classic-control]'
pip install 'gymnasium[mujoco]'
pip install swig
pip install 'gymnasium[box2d]'
```

## Step 5

Register the environment (replace iaiiot with whatever name you use from step 1).

```console
python -m ipykernel install --user --name iaiiot --display-name "iaiiot"
```

## Use the new environment

To launch jupyterlab using browser, type
```console
jupyter lab
```

Alternatively, if you want to use visual studio code, install VSC and jupyter extension. Then select this environment.
