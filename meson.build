project('snake', 'cpp')

qt5 = import('qt5')
qt5_deps = dependency('qt5', modules: ['Gui', 'Core', 'Widgets'])


# Qt5 preprocessing
moc_headers = ['ui_controllers/main_window.h', 'ui_controllers/subtitle_list_view_model.h', 'application_state.h', 'ui_controllers/waveform.h']
ui_files = ['ui_forms/mainwindow.ui']
moc_files = qt5.preprocess(moc_headers: moc_headers, ui_files: ui_files)


# Snake cpp files
snake_cpp_files = ['main.cpp', 'subtitle_list/subtitle_list.cpp', 'subtitle_formats/srt_parser.cpp', 'ui_controllers/main_window.cpp', 'ui_controllers/subtitle_list_view_model.cpp', 'application_state.cpp', 'ui_controllers/waveform.cpp']


dependencies = [qt5_deps]
executable('snake', snake_cpp_files, moc_files, dependencies: dependencies, cpp_args: '-std=c++17')
