project('snake', 'cpp')

qt5 = import('qt5')
qt5_deps = dependency('qt5', modules: ['Gui', 'Core', 'Widgets'])

mpv_dep = dependency('mpv')

# mpv_inc = include_directories('/home/francesco/Projects/mpv/')

# mpv_dep = meson.get_compiler('cpp').find_library('libmpv', dirs: '/home/francesco/Projects/mpv/build/')

# Qt5 preprocessing
moc_headers = [ 'ui_controllers/main_window.h'
              , 'ui_controllers/subtitle_dialog_editor.h'
              , 'application_state.h'
              , 'media_renderer.h'
              , 'ui_controllers/waveform.h'
              , 'ui_controllers/subtitle_list_view.h'
              , 'ui_controllers/video_player.h'
              ]

ui_files = ['ui_forms/mainwindow.ui'
           ]

moc_files = qt5.preprocess(moc_headers: moc_headers, ui_files: ui_files)


# Snake cpp files
snake_cpp_files = [ 'main.cpp'
                  , 'subtitle_list/subtitle_list.cpp'
                  , 'subtitle_formats/srt_parser.cpp'
                  , 'ui_controllers/main_window.cpp'
                  , 'ui_controllers/subtitle_dialog_editor.cpp'
                  , 'application_state.cpp'
                  , 'media_renderer.cpp'
                  , 'ui_controllers/waveform.cpp'
                  , 'subtitle_list/time_interval.cpp'
                  , 'utils/time_utils.cpp'
                  , 'ui_controllers/subtitle_list_view.cpp'
                  , 'ui_controllers/video_player.cpp'
                  ]


dependencies = [qt5_deps, mpv_dep]
executable('snake', snake_cpp_files, moc_files, dependencies: dependencies, cpp_args: ['-std=c++17', '-g'])
