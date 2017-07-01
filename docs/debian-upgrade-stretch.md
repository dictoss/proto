
# upgrade debian-9(stretch) from debian-8(jessie)

## openvpn

- upgrade 2.4.0 (stretch) from 2.3.4

  - advantage
  
    - support cipher ECDHE.
    
  - **TROUBLE**

    - if use crl-verify, must re-generate crl-verify.
    - reference

      - https://forums.openvpn.net/viewtopic.php?t=23166

## LXC

- upgrade 2.0.7 (stretch) from 1.0.6 (jessie)

  - **TROUBLE**

    - do not reach network guest LXC.

      - create /etc/default/lxc-net, and write USE_LXC_BRIDGE="true"
      
      - reference

        - https://wiki.debian.org/LXC#Changes_between_.22Jessie.22_and_.22Stretch.22

## vim

- upgrade 8.0 (stretch) from 7.4 (jessie)

  - **TROUBLE**

    - do not paste on write click event.
    - for vim-8.0, change default write click event. 

    - set ~/.vimrc to fallback vim-7.4

     - `set mouse=`
     - After restart vim, can paste on write click event.
