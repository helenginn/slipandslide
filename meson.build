project('slipnslide', 'cpp')
qt5 = import('qt5')
crystfel = dependency('crystfel')
gsl = dependency('gsl')
hdf5 = dependency('hdf5', language: 'cpp')
qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])
helen3d_dep = dependency('helen3d')

moc_files = qt5.preprocess(moc_headers : [
'src/DetectorView.h',
'src/Refine.h',
'src/Overview.h',
],
moc_extra_arguments: ['-DMAKES_MY_MOC_HEADER_COMPILE'])

executable('slipnslide', 
'src/main.cpp', 
'src/DetectorView.cpp', 
'src/Line.cpp', 
'src/Refine.cpp', 
'src/Overview.cpp', 
'src/SlipPanel.cpp', 
'src/Splattice.cpp', 
moc_files, dependencies: [hdf5, qt5_dep, gsl, crystfel, helen3d_dep], install: true)

