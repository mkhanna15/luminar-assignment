set buildType="RELEASE"
set cleanBuild=true
set generateDocs=false

set oldpwd=%cd%

cd %~dp0/..
if "%cleanBuild%" == "true" ( rmdir /s /q build )

mkdir build

cmake -Bbuild -H. -A"x64" -G"Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=%buildType% || EXIT /B 1
cmake --build ./build --config %buildType% || EXIT /B 1

cd %oldpwd%