# mod_korean

## Description

mod_korean 확장은 한국어 환경에서 PHP 개발을 위한 여러가지 PHP api를 제공 합니다.

이 extension은 현재 더이상 새로운 feature를 추가하고 있지 않으며, 기존의 사용자들을 위하여, PHP update에 대한 변경 사항 및 기존 기능의 버그 수정만 이루어지고 있습니다.

## Installation

```bash
[root@host mod_korean-1.0.1]$ phpize
[root@host mod_korean-1.0.1]$ ./configure
[root@host mod_korean-1.0.1]$ make && make install
```

## Configuration

php.ini 에 다음의 설정을 추가해 줍니다.

```ini
extension = korean.so
```

## Contributors
 * JoungKyun.Kim
 * HyunSoo Choi <http://baby.web119.com> - Donate UTF8 internal API code
 * ChangHyun Bang <winchild at kldp.org>
 * Junho Choi <cjh at kr.freebsd.org>
