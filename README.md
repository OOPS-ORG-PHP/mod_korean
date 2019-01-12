# mod_korean

## Description

mod_korean 확장은 한국어 환경에서 PHP 개발을 위한 여러가지 PHP api를 제공 합니다.

이 extension은 현재 더이상 새로운 feature를 추가하고 있지 않으며, 기존의 사용자들을 위하여, PHP update에 대한 변경 사항 및 기존 기능의 버그 수정만 이루어지고 있습니다.

PHP 7 발표 이후, PHP extension 의 구조 변경 때문에, PHP 버전에 따른 mod_korean의 최신 버전은 다음과 같습니다.

 * PHP 7 - [mod_korean 1.0.2](https://github.com/OOPS-ORG-PHP/mod_korean/releases/tag/1.0.2)
 * PHP 4 or PHP5 - [mod_korean 0.0.5](https://github.com/OOPS-ORG-PHP/mod_korean/releases/tag/0.1.5)

## Installation

```bash
[root@host mod_korean-1.0.2]$ phpize
[root@host mod_korean-1.0.2]$ # --with-libdir 옵션은 64bit 환경에서만 지정합니다.
[root@host mod_korean-1.0.2]$ ./configure --with-libdir=lib64 --enable-korean --enable-korean-gd=builtin
[root@host mod_korean-1.0.2]$ make
```

## Test

PHP 는 빌드된 모듈이 잘 작동하는지 test 를 할 수 있습니다.
참고로, ___mod_korean___ extension 의 정상적인 테스트를 위해서는 php cli 환경에서 ___allow_url_fopen___ 이 enable 되어 있어야 합니다.


```bash
[root@host mod_korean-1.0.2]$ make test PHP_EXECUTABLE=/usr/bin/php
```

또는, install 전에 빌드한 extension을 직접 테스트 하려면 다음과 같이 가능 합니다.

```bash
[root@host mod_korean-1.0.2]$ php -d "extension_dir=./modules/" -d "extension=korean.so" some.php
```

## Installation

PHP 의 extension build의 installation 은 phpize 에서 extension dir을 미리 결정해 놓기 때문에 다음 명령으로 간단히 처리 됩니다.

```bash
[root@host mod_korean-1.0.2]$ make install
```

설치 시에, 테스트나 패키징을 위한 chroot 가 필요 하다면, 다음과 같이 사용 하십시오.

```bash
[root@host mod_korean-1.0.2]$ make install DESTDIR="/want/path"
```

또는, 그냥 ___modules___ 디렉토리에 있는 so file을 copy 하셔도 무방 합니다.


## Configuration

php.ini 에 다음의 설정을 추가해 줍니다.

```ini
extension = korean.so
```

## Reference

https://www.gitbook.com/book/joungkyun/php-mod_korean-extension-reference/details

## Contributors
 * JoungKyun.Kim
 * HyunSoo Choi &lt;http://baby.web119.com&gt; - Donate UTF8 internal API code
 * ChangHyun Bang &lt;winchild at kldp.org&gt;
 * Junho Choi &lt;cjh at kr.freebsd.org&gt;
