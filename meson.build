project('slipnslide', 'cpp')
qt5 = import('qt5')
crystfel = dependency('crystfel')
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
moc_files, dependencies: [qt5_dep, crystfel, helen3d_dep], install: true)

