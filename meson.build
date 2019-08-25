project('slipnslide', 'cpp')
qt5 = import('qt5')
crystfel = dependency('crystfel')
qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])

moc_files = qt5.preprocess(moc_headers : [
'src/DetectorView.h',
'src/SlipGL.h',
],
                           moc_extra_arguments: ['-DMAKES_MY_MOC_HEADER_COMPILE'])

executable('slipnslide', 
'src/main.cpp', 
'src/mat3x3.cpp', 
'src/vec3.cpp', 
'src/DetectorView.cpp', 
'src/SlipGL.cpp', 
'src/SlipPanel.cpp', 
'src/SlipObject.cpp', 
moc_files, dependencies: [qt5_dep, crystfel], install: true)

#

