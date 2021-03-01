
import argparse
import enum
import glob
import ninja_syntax
import os
import platform as platform_info
import subprocess
import sys

Platform = enum.Enum('Platform', 'msvc gcc')
BuildType = enum.Enum('BuildType', 'debug release master')

def detect_platform():
	return { 'Windows': Platform.msvc, 'Linux': Platform.gcc }[platform_info.system()]

def get_platform_writer(platform):
	return { Platform.msvc: PlatformMSVC(), Platform.gcc: PlatformGCC() }[platform]

def get_build_type_name(build_type):
	return { BuildType.debug: 'Debug', BuildType.release: 'Release', BuildType.master: 'Master' }[build_type]

def get_code_dir(project_name):
	return os.path.join('Code', project_name, '')

def get_build_dir(project_name, platform_name, build_type):
	return os.path.join('Build', project_name, platform_name, get_build_type_name(build_type), '')

def get_deploy_dir(project_name, platform_name, build_type):
	return os.path.join('Deploy', project_name, platform_name, get_build_type_name(build_type), '')

def get_cpp_files(dir):
	return glob.glob(os.path.join(dir, '**/*.cpp'), recursive = True)

def get_c_files(dir):
	return glob.glob(os.path.join(dir, '**/*.c'), recursive = True)
	
def get_test_files(dir):
	return [os.path.relpath(f, dir) for f in glob.glob(os.path.join(dir, '**/*.test.cpp'), recursive = True)]

def get_file_stem(filename):
	return os.path.splitext(os.path.basename(filename))[0]

def join_file(*args):
	return os.path.join(*args)

def join_dir(*args):
	return os.path.join(*args, '')

class ProjectStaticLib:
	def __init__(self):
		self.project_name = ''
		self.code_dir = ''
		self.build_dir = ''
		self.deploy_dir = ''
		self.src_files = []
		self.defines = []
		self.inc_dirs = []

	@classmethod
	def from_name(cls, project_name, platform, build_type):
		lib = cls()
		lib.project_name = project_name
		lib.code_dir = get_code_dir(project_name)
		lib.build_dir = get_build_dir(project_name, platform.get_platform_name(), build_type)
		lib.deploy_dir = get_deploy_dir(project_name, platform.get_platform_name(), build_type)
		lib.src_files = get_cpp_files(lib.code_dir) + get_c_files(lib.code_dir)
		return lib

class ProjectExe:
	def __init__(self):
		self.project_name = ''
		self.code_dir = ''
		self.build_dir = ''
		self.deploy_dir = ''
		self.src_files = []
		self.defines = []
		self.inc_dirs = []
		self.standard_libs = []
		self.libs = []
		self.lib_dirs = []

	@classmethod
	def from_name(cls, project_name, platform, build_type):
		exe = cls()
		exe.project_name = project_name
		exe.code_dir = get_code_dir(project_name)
		exe.build_dir = get_build_dir(project_name, platform.get_platform_name(), build_type)
		exe.deploy_dir = get_deploy_dir(project_name, platform.get_platform_name(), build_type)
		exe.src_files = get_cpp_files(exe.code_dir) + get_c_files(exe.code_dir)
		return exe

class PlatformMSVC:
	
	def get_platform_name(self):
		return 'MSVC'
	
	def get_lib_name(self, name):
		return name + '.lib'

	def write_rules(self, n):
		n.rule('cc', 'cl.exe -c $cc_flags $cc_build_flags $cc_warning_level $cc_defines $cc_out_obj $cc_out_pdb $cc_in_includes $cc_in_files')
		n.rule('ar', 'lib.exe $ar_flags $ar_out_file $ar_in_files')
		n.rule('link', 'link.exe $link_flags $link_build_flags $link_out_exe $link_out_pdb $link_in_lib_dirs $link_in_files')

	def get_cc_flags(self):
		return '/nologo /Gy /EHsc /WX /std:c++17 /Zc:__cplusplus /utf-8'

	def get_cc_build_flags(self, build_type):
		return { BuildType.debug: '/MTd /Od /Zi', BuildType.release: '/MT /Ox /Zi', BuildType.master: '/MT /Ox' }[build_type]

	def get_cc_defines(self, defines):
		return ' '.join('/D' + d for d in defines)
	
	def get_cc_warning_level(self, warning_level):
		return '/W' + str(warning_level)

	def get_cc_out_obj(self, path):
		return '/Fo' + path

	def get_cc_out_pdb(self, path):
		return '/Fd' + path

	def get_cc_in_includes(self, paths):
		return ' '.join('/I' + p for p in paths)

	def write_cc_build(self, n, build_type, in_src, in_includes, out_obj, out_pdb, defines, warning_level):
		in_deps = [in_src]
		out_deps = [out_obj, out_pdb]
		n.build(out_deps, 'cc', in_deps,
			variables = [
				('cc_flags', self.get_cc_flags()),
				('cc_build_flags', self.get_cc_build_flags(build_type)),
				('cc_warning_level', self.get_cc_warning_level(warning_level)),
				('cc_defines', self.get_cc_defines(defines)),
				('cc_out_obj', self.get_cc_out_obj(out_obj)),
				('cc_out_pdb', self.get_cc_out_pdb(out_pdb)),
				('cc_in_includes', self.get_cc_in_includes(in_includes)),
				('cc_in_files', in_src)
			])

	def get_ar_flags(self):
		return '/NOLOGO'

	def get_ar_out_file(self, path):
		return '/OUT:' + path

	def write_ar_build(self, n, in_objs, out_lib):
		in_deps = in_objs
		out_deps = [out_lib]
		n.build(out_deps, 'ar', in_deps,
			variables = [
				('ar_flags', self.get_ar_flags()),
				('ar_out_file', self.get_ar_out_file(out_lib)),
				('ar_in_files', ' '.join(in_objs))
			])

	def get_link_flags(self):
		return '/NOLOGO /OPT:REF /SUBSYSTEM:CONSOLE'

	def get_link_build_flags(self, build_type):
		return { BuildType.debug: '/DEBUG:FULL', BuildType.release: '/DEBUG:FULL', BuildType.master: '/DEBUG:NONE' }[build_type]

	def get_link_out_exe(self, path):
		return '/OUT:' + path

	def get_link_out_pdb(self, path):
		return '/PDB:' + path

	def get_link_in_lib_dirs(self, paths):
		return ' '.join('/LIBPATH:' + p for p in paths)

	def write_link_build(self, n, build_type, in_objs, in_standard_libs, in_libs, in_lib_dirs, out_exe, out_pdb):
		in_deps = in_objs + in_libs
		out_deps = [out_exe, out_pdb]
		n.build(out_deps, 'link', in_deps,
			variables = [
				('link_flags', self.get_link_flags()),
				('link_build_flags', self.get_link_build_flags(build_type)),
				('link_out_exe', self.get_link_out_exe(out_exe)),
				('link_out_pdb', self.get_link_out_pdb(out_pdb)),
				('link_in_lib_dirs', self.get_link_in_lib_dirs(in_lib_dirs)),
				('link_in_files', ' '.join(in_objs + in_libs + in_standard_libs))
			])
			
	def write_static_lib(self, n, build_type, lib, warning_level = '4'):
		src_paths = lib.src_files
		inc_paths = lib.inc_dirs
		obj_paths = [join_file(lib.build_dir, get_file_stem(f)) + '.obj' for f in lib.src_files]
		pdb_paths = [join_file(lib.deploy_dir, get_file_stem(f)) + '.pdb' for f in lib.src_files]

		for src, obj, pdb in zip(src_paths, obj_paths, pdb_paths):
			self.write_cc_build(n, build_type, src, inc_paths, obj, pdb, lib.defines, warning_level)

		lib_path = join_file(lib.deploy_dir, lib.project_name + '.lib')
		self.write_ar_build(n, obj_paths, lib_path)

	def write_exe(self, n, build_type, exe, warning_level = '4'):
		src_paths = exe.src_files
		inc_paths = exe.inc_dirs
		obj_paths = [join_file(exe.build_dir, get_file_stem(f)) + '.obj' for f in exe.src_files]
		pdb_paths = [join_file(exe.build_dir, get_file_stem(f)) + '.pdb' for f in exe.src_files]

		for src, obj, pdb in zip(src_paths, obj_paths, pdb_paths):
			self.write_cc_build(n, build_type, src, inc_paths, obj, pdb, exe.defines, warning_level)

		exe_path = join_file(exe.deploy_dir, exe.project_name + '.exe')
		pdb_final_path = join_file(exe.deploy_dir, exe.project_name + '.pdb')
		self.write_link_build(n, build_type, obj_paths, exe.standard_libs, exe.libs, exe.lib_dirs, exe_path, pdb_final_path)
		
	def write(self, n, build_type):

		self.write_rules(n)

		stb = ProjectStaticLib.from_name('stbimage', self, build_type)
		stb.defines = [ 'STBI_WINDOWS_UTF8' ]
		self.write_static_lib(n, build_type, stb, '0')

		glm = ProjectStaticLib.from_name('glm', self, build_type)
		glm.defines = [ 'GLM_FORCE_EXPLICIT_CTOR', 'GLM_FORCE_XYZW_ONLY', 'GLM_FORCE_SIZE_T_LENGTH', 'GLM_FORCE_SILENT_WARNINGS' ]
		glm.inc_dirs = [ glm.code_dir ]
		self.write_static_lib(n, build_type, glm, '4')

		sdl = ProjectStaticLib.from_name('sdl', self, build_type)
		sdl.defines = [ 'HAVE_LIBC', 'SDL_DYNAMIC_API=0', 'DECLSPEC=' ]
		sdl.src_files =  [
			"src/atomic/SDL_atomic.c",
			"src/atomic/SDL_spinlock.c",
			"src/audio/directsound/SDL_directsound.c",
			"src/audio/disk/SDL_diskaudio.c",
			"src/audio/dummy/SDL_dummyaudio.c",
			"src/audio/SDL_audio.c",
			"src/audio/SDL_audiocvt.c",
			"src/audio/SDL_audiodev.c",
			"src/audio/SDL_audiotypecvt.c",
			"src/audio/SDL_mixer.c",
			"src/audio/SDL_wave.c",
			"src/audio/winmm/SDL_winmm.c",
			"src/audio/wasapi/SDL_wasapi.c",
			"src/audio/wasapi/SDL_wasapi_win32.c",
			"src/core/windows/SDL_windows.c",
			"src/core/windows/SDL_xinput.c",
			"src/cpuinfo/SDL_cpuinfo.c",
			"src/dynapi/SDL_dynapi.c",
			"src/events/SDL_clipboardevents.c",
			"src/events/SDL_displayevents.c",
			"src/events/SDL_dropevents.c",
			"src/events/SDL_events.c",
			"src/events/SDL_gesture.c",
			"src/events/SDL_keyboard.c",
			"src/events/SDL_mouse.c",
			"src/events/SDL_quit.c",
			"src/events/SDL_touch.c",
			"src/events/SDL_windowevents.c",
			"src/file/SDL_rwops.c",
			"src/filesystem/windows/SDL_sysfilesystem.c",
			"src/haptic/SDL_haptic.c",
			"src/haptic/windows/SDL_dinputhaptic.c",
			"src/haptic/windows/SDL_windowshaptic.c",
			"src/haptic/windows/SDL_xinputhaptic.c",
			"src/hidapi/windows/hid.c",
			"src/joystick/hidapi/SDL_hidapijoystick.c",
			"src/joystick/hidapi/SDL_hidapi_ps4.c",
			"src/joystick/hidapi/SDL_hidapi_switch.c",
			"src/joystick/hidapi/SDL_hidapi_xbox360.c",
			"src/joystick/hidapi/SDL_hidapi_xboxone.c",
			"src/joystick/SDL_gamecontroller.c",
			"src/joystick/SDL_joystick.c",
			"src/joystick/windows/SDL_dinputjoystick.c",
			"src/joystick/windows/SDL_mmjoystick.c",
			"src/joystick/windows/SDL_windowsjoystick.c",
			"src/joystick/windows/SDL_xinputjoystick.c",
			"src/libm/e_atan2.c",
			"src/libm/e_exp.c",
			"src/libm/e_fmod.c",
			"src/libm/e_log.c",
			"src/libm/e_log10.c",
			"src/libm/e_pow.c",
			"src/libm/e_rem_pio2.c",
			"src/libm/e_sqrt.c",
			"src/libm/k_cos.c",
			"src/libm/k_rem_pio2.c",
			"src/libm/k_sin.c",
			"src/libm/k_tan.c",
			"src/libm/s_atan.c",
			"src/libm/s_copysign.c",
			"src/libm/s_cos.c",
			"src/libm/s_fabs.c",
			"src/libm/s_floor.c",
			"src/libm/s_scalbn.c",
			"src/libm/s_sin.c",
			"src/libm/s_tan.c",
			"src/loadso/windows/SDL_sysloadso.c",
			"src/power/SDL_power.c",
			"src/power/windows/SDL_syspower.c",
			"src/render/direct3d11/SDL_shaders_d3d11.c",
			"src/render/direct3d/SDL_render_d3d.c",
			"src/render/direct3d11/SDL_render_d3d11.c",
			"src/render/direct3d/SDL_shaders_d3d.c",
			"src/render/opengl/SDL_render_gl.c",
			"src/render/opengl/SDL_shaders_gl.c",
			"src/render/opengles2/SDL_render_gles2.c",
			"src/render/opengles2/SDL_shaders_gles2.c",
			"src/render/SDL_d3dmath.c",
			"src/render/SDL_render.c",
			"src/render/SDL_yuv_sw.c",
			"src/render/software/SDL_blendfillrect.c",
			"src/render/software/SDL_blendline.c",
			"src/render/software/SDL_blendpoint.c",
			"src/render/software/SDL_drawline.c",
			"src/render/software/SDL_drawpoint.c",
			"src/render/software/SDL_render_sw.c",
			"src/render/software/SDL_rotate.c",
			"src/SDL.c",
			"src/SDL_assert.c",
			"src/SDL_dataqueue.c",
			"src/SDL_error.c",
			"src/SDL_hints.c",
			"src/SDL_log.c",
			"src/sensor/dummy/SDL_dummysensor.c",
			"src/sensor/SDL_sensor.c",
			"src/stdlib/SDL_getenv.c",
			"src/stdlib/SDL_iconv.c",
			"src/stdlib/SDL_malloc.c",
			"src/stdlib/SDL_qsort.c",
			"src/stdlib/SDL_stdlib.c",
			"src/stdlib/SDL_string.c",
			"src/thread/generic/SDL_syscond.c",
			"src/thread/SDL_thread.c",
			"src/thread/windows/SDL_sysmutex.c",
			"src/thread/windows/SDL_syssem.c",
			"src/thread/windows/SDL_systhread.c",
			"src/thread/windows/SDL_systls.c",
			"src/timer/SDL_timer.c",
			"src/timer/windows/SDL_systimer.c",
			"src/video/dummy/SDL_nullevents.c",
			"src/video/dummy/SDL_nullframebuffer.c",
			"src/video/dummy/SDL_nullvideo.c",
			"src/video/SDL_blit.c",
			"src/video/SDL_blit_0.c",
			"src/video/SDL_blit_1.c",
			"src/video/SDL_blit_A.c",
			"src/video/SDL_blit_auto.c",
			"src/video/SDL_blit_copy.c",
			"src/video/SDL_blit_N.c",
			"src/video/SDL_blit_slow.c",
			"src/video/SDL_bmp.c",
			"src/video/SDL_clipboard.c",
			"src/video/SDL_egl.c",
			"src/video/SDL_fillrect.c",
			"src/video/SDL_pixels.c",
			"src/video/SDL_rect.c",
			"src/video/SDL_RLEaccel.c",
			"src/video/SDL_shape.c",
			"src/video/SDL_stretch.c",
			"src/video/SDL_surface.c",
			"src/video/SDL_video.c",
			"src/video/SDL_vulkan_utils.c",
			"src/video/SDL_yuv.c",
			"src/video/windows/SDL_windowsclipboard.c",
			"src/video/windows/SDL_windowsevents.c",
			"src/video/windows/SDL_windowsframebuffer.c",
			"src/video/windows/SDL_windowskeyboard.c",
			"src/video/windows/SDL_windowsmessagebox.c",
			"src/video/windows/SDL_windowsmodes.c",
			"src/video/windows/SDL_windowsmouse.c",
			"src/video/windows/SDL_windowsopengl.c",
			"src/video/windows/SDL_windowsopengles.c",
			"src/video/windows/SDL_windowsshape.c",
			"src/video/windows/SDL_windowsvideo.c",
			"src/video/windows/SDL_windowsvulkan.c",
			"src/video/windows/SDL_windowswindow.c",
			"src/video/yuv2rgb/yuv_rgb.c",
		]
		sdl.src_files = [ join_file(sdl.code_dir, f) for f in sdl.src_files ]
		sdl.inc_dirs = [
			join_dir(sdl.code_dir, 'include')
		]
		self.write_static_lib(n, build_type, sdl, '3')

		sdlmain = ProjectStaticLib.from_name('sdlmain', self, build_type)
		sdlmain.defines = sdl.defines
		sdlmain.src_files = [
			join_file(sdl.code_dir, 'src/main/windows/SDL_windows_main.c')
		]
		sdlmain.inc_dirs = [
			join_dir(sdl.code_dir, 'include')
		]
		self.write_static_lib(n, build_type, sdlmain, '3')

		sdlmixer = ProjectStaticLib.from_name('sdlmixer', self, build_type)
		sdlmixer.defines = sdl.defines + [ 'MUSIC_WAV' ]
		sdlmixer.inc_dirs = [
			join_dir(sdl.code_dir, 'include')
		]
		self.write_static_lib(n, build_type, sdlmixer, '3')

		snek = ProjectExe.from_name('snek', self, build_type)
		snek.defines = stb.defines + glm.defines + sdl.defines + [ 'MUSIC_WAV' ]
		snek.inc_dirs = [
			stb.code_dir,
			glm.code_dir,
			join_dir(sdl.code_dir, 'include'),
			sdlmixer.code_dir,
		]
		snek.libs = [
			join_file(stb.deploy_dir, self.get_lib_name(stb.project_name)),
			join_file(sdlmain.deploy_dir, self.get_lib_name(sdlmain.project_name)),
			join_file(sdl.deploy_dir, self.get_lib_name(sdl.project_name)),
			join_file(sdlmixer.deploy_dir, self.get_lib_name(sdlmixer.project_name)),
		]
		snek.standard_libs = [ 'User32.lib', 'Shell32.lib', 'Ole32.lib', 'OpenGL32.lib', 'gdi32.lib', 'Winmm.lib', 'Advapi32.lib', 'Version.lib', 'Imm32.lib', 'Setupapi.lib', 'OleAut32.lib' ]
		self.write_exe(n, build_type, snek)

class PlatformGCC:

	def get_platform_name(self):
		return 'GCC'

	def get_lib_name(self, name):
		return 'lib' + name + '.a'
	
	def write_rules(self, n):
		n.rule('cc', 'g++ -c $cc_flags $cc_build_flags $cc_defines $cc_out_obj $cc_in_includes $cc_in_files')
		n.rule('ar', 'ar $ar_flags $ar_out_file $ar_in_files')
		n.rule('ld', 'g++ $ld_flags $ld_build_flags $ld_out_exe $ld_in_lib_dirs $ld_in_files $ld_in_libs')
	
	def get_cc_flags(self):
		return '-Wall -Werror -std=c++17'
	
	def get_cc_build_flags(self, build_type):
		return { BuildType.debug: '-g -Og', BuildType.release: '-g -O3', BuildType.master: '-O3' }[build_type]

	def get_cc_defines(self, defines):
		return ' '.join('-D' + d for d in defines)

	def get_cc_out_obj(self, path):
		return '-o' + path
	
	def get_cc_in_includes(self, paths):
		return ' '.join('-I' + p for p in paths)
	
	def write_cc_build(self, n, build_type, in_src, in_includes, out_obj, defines):
		in_deps = [in_src]
		out_deps = [out_obj]
		n.build(out_deps, 'cc', in_deps,
			variables = [
				('cc_flags', self.get_cc_flags()),
				('cc_build_flags', self.get_cc_build_flags(build_type)),
				('cc_defines', self.get_cc_defines(defines)),
				('cc_out_obj', self.get_cc_out_obj(out_obj)),
				('cc_in_includes', self.get_cc_in_includes(in_includes)),
				('cc_in_files', in_src)
			])

	def get_ar_flags(self):
		return 'ru'
	
	def get_ar_out_file(self, path):
		return '-o ' + path

	def write_ar_build(self, n, in_objs, out_lib):
		in_deps = in_objs
		out_deps = [out_lib]
		n.build(out_deps, 'ar', in_deps,
			variables = [
				('ar_flags', self.get_ar_flags()),
				('ar_out_file', self.get_ar_out_file(out_lib)),
				('ar_in_files', ' '.join(in_objs))
			])
	
	def get_link_flags(self):
		return '-Wall -Werror -std=c++17'
	
	def get_link_build_flags(self, build_type):
		return { BuildType.debug: '-g -Og', BuildType.release: '-g -O3', BuildType.master: '-O3' }[build_type]
	
	def get_link_out_exe(self, path):
		return '-o ' + path
	
	def get_link_in_lib_dirs(self, paths):
		return ' '.join('-L' + p for p in paths)

	def get_link_in_libs(self, paths):
		return ' '.join('-l:' + p for p in paths)

	def get_link_in_standard_libs(self, paths):
		return ' '.join('-l' + p for p in paths)

	def write_link_build(self, n, build_type, in_objs, in_standard_libs, in_libs, in_lib_dirs, out_exe):
		in_deps = in_objs + in_libs
		out_deps = [out_exe]
		n.build(out_deps, 'ld', in_deps,
			variables = [
				('ld_flags', self.get_link_flags()),
				('ld_build_flags', self.get_link_build_flags(build_type)),
				('ld_out_exe', self.get_link_out_exe(out_exe)),
				('ld_in_lib_dirs', self.get_link_in_lib_dirs(in_lib_dirs)),
				('ld_in_files', ' '.join(in_objs)),
				('ld_in_libs', self.get_link_in_libs(in_libs) + ' ' + self.get_link_in_standard_libs(in_standard_libs)),
			])

	def write_static_lib(self, n, build_type, lib):
		src_paths = lib.src_files
		inc_paths = lib.inc_dirs
		obj_paths = [join_file(lib.build_dir, get_file_stem(f) + '.o') for f in lib.src_files]

		for src, obj in zip(src_paths, obj_paths):
			self.write_cc_build(n, build_type, src, inc_paths, obj, lib.defines)

		lib_path = join_file(lib.deploy_dir, self.get_lib_name(lib.project_name))
		self.write_ar_build(n, obj_paths, lib_path)

	def write_exe(self, n, build_type, exe):
		src_paths = exe.src_files
		inc_paths = exe.inc_dirs
		obj_paths = [join_file(exe.build_dir, get_file_stem(f) + '.o') for f in exe.src_files]

		for src, obj in zip(src_paths, obj_paths):
			self.write_cc_build(n, build_type, src, inc_paths, obj, exe.defines)

		exe_path = join_file(exe.deploy_dir, exe.project_name)
		self.write_link_build(n, build_type, obj_paths, exe.standard_libs, exe.libs, exe.lib_dirs, exe_path)

	def write(self, n, build_type):
		self.write_rules(n)
		
		# ...

def write(platform, build_type, build_file_name):
	try:
		with open(build_file_name, 'w') as build_file:
			n = ninja_syntax.Writer(build_file)
			platform.write(n, build_type)

	except IOError:
		print('failed to write build file: ', build_file_name)
		raise

def main():

	platform = detect_platform()
	platform_writer = get_platform_writer(platform)

	parser = argparse.ArgumentParser()

	parser.add_argument('--build_type',
		required = True,
		choices = [i.name for i in BuildType],
		action = 'store',
		dest = 'build_type',
		help = 'build for \'debug\' or \'release\' mode (affects optimization, debug symbols, etc.)')

	parser.add_argument('--no_build',
		action = 'store_true',
		dest = 'no_build',
		default = False,
		help = 'generate the ninja build file, but don\'t run the build')
	
	args = parser.parse_args()
	build_type = BuildType[args.build_type]

	try:
		build_file_name = 'build-' + platform.name + '-' + args.build_type + '.ninja'
		write(platform_writer, build_type, build_file_name)

		print('created build file: ', build_file_name)

	except:
		print('failed to generate build file!')
		raise

	if not args.no_build:
		build_result = subprocess.call(['ninja', '-f', build_file_name])

		if build_result != 0:
			print('build failed!')
			return

	print('build succeeded')

	if platform.name == 'msvc':
		copy_result = subprocess.call(['robocopy', 
			'/v', '/w:1', '/mir', 
			'Data/snek', 
			get_deploy_dir('snek', platform_writer.get_platform_name(), build_type) + '/data'])
			
	
main()
