#!lua

--
-- Centaurean SpookyHash
--
-- Copyright (c) 2015, Guillaume Voirin
-- All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions are met:
--
--     1. Redistributions of source code must retain the above copyright notice, this
--        list of conditions and the following disclaimer.
--
--     2. Redistributions in binary form must reproduce the above copyright notice,
--        this list of conditions and the following disclaimer in the documentation
--        and/or other materials provided with the distribution.
--
--     3. Neither the name of the copyright holder nor the names of its
--        contributors may be used to endorse or promote products derived from
--        this software without specific prior written permission.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
-- DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
-- FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
-- DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
-- SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
-- CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
-- OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
-- OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--
-- 2/05/15 23:04
--

-- LTO with static libs
premake.tools.gcc.ar = "gcc-ar"

newoption {
	trigger = "toolset",
	value = "TOOLSET",
	description = "Choose a custom toolset",
	allowed = {
	  { "clang", "Clang" },
	  { "gcc", "GCC" },
	  { "msc", "MSC" },
	  { "msc-llvm-vs2014", "LSC/LLVM"}
	}
}

newoption {
	trigger = "lto",
	value = "LTO",
	description = "Link time optimisations",
	allowed = {
	  { "on", "On" },
	  { "off", "Off" },
	}
}

newoption {
	trigger = "arch",
	value = "Architecture",
	description = "Select architecture",
	allowed = {
	  { "x32", "x32" },
  	  { "x64", "x64" },
  	  { "arm", "ARM" },
  	  { "native", "Native" },
	}
}

newoption {
	trigger = "gccprefix",
	value = "GCC prefix",
	description = "Select GCC prefix",
	allowed = {
	  { "aarch64-linux-gnu-", "aarch64-linux-gnu-" },
  	  { "arm-linux-gnueabi-", "arm-linux-gnueabi-" },
  	  { "powerpc-linux-gnu-", "powerpc-linux-gnu-" },
  	  { "", "" },
	}
}

if _OPTIONS["toolset"] == "clang" then
	toolset "clang"
	io.write("Using Clang toolset\n")
end
if _OPTIONS["toolset"] == "gcc" then
	toolset "gcc"
	io.write("Using GCC toolset\n")
end
if _OPTIONS["toolset"] == "msc" then
	toolset "msc"
	io.write("Using MSC toolset\n")
end
if _OPTIONS["toolset"] == "msc-llvm-vs2014" then
	toolset "msc-llvm-vs2014"
	io.write("Using MSC/LLVM toolset\n")
end

if _OPTIONS["gccprefix"] then
	gccprefix(_OPTIONS["gccprefix"])
end

-- Submodules update
os.execute("git submodule update --init --recursive")

solution "SpookyHash"
	configurations { "Release" }
	if _OPTIONS["lto"] == "off" then
		flags { "NoFramePointer" }
		io.write("Disabling link time optimizations\n")
	else
		flags { "NoFramePointer", "LinkTimeOptimization" }
	end
	optimize "Speed"
	cdialect "C99"
	warnings "Extra"

	if _OPTIONS["arch"] then
		if _OPTIONS["arch"] == "x32" then
			architecture "x32"
		end
		if _OPTIONS["arch"] == "x64" then
			architecture "x64"
		end
		if _OPTIONS["arch"] == "arm" then
			architecture "ARM"
		end
		io.write("Using " .. _OPTIONS["arch"] .. " architecture\n")
	else
		-- Prefer 64bit binary if system architecture is 64bit
 		if os.is64bit() then
			architecture "x64"
			io.write("Using x64 architecture\n")
		end
	end

	project "spookyhash-static"
		targetname ("spookyhash")
		kind "StaticLib"
		language "C"
		files {
			"../src/*.h",
			"../src/*.c"
		}

	project "spookyhash-shared"
		targetname ("spookyhash")
		kind "SharedLib"
		language "C"
		files {
			"../src/*.h",
			"../src/*.c"
		}

	project "spookyhash-test"
		kind "ConsoleApp"
		language "C"
		files {
			"../test/libs/**.h",
			"../test/libs/**.c",
			"../test/src/*.h",
			"../test/src/*.c"
		}
		links { "spookyhash-static" }
