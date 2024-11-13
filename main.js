document.addEventListener('DOMContentLoaded', () => {
    const navbar = document.querySelector("div#navbar");
    const readmeContent = document.getElementById('readme-content');
    const changelogContent = document.getElementById('changelog-content');
    const configForm = document.getElementById('config-form');
    const copyButton = document.getElementById('copy-button');
    const configOutput = document.getElementById('config-output');
    marked.use({ breaks: true });
    
    const fetchFile = async (url, targetElement) => {
        try {
            const response = await fetch(url);
            if (!response.ok) throw new Error(`${url} not found`);
            const text = await response.text();
            targetElement.innerHTML = marked.parse(text);
        } catch (error) {
            console.error(`Error fetching ${url}:`, error);
        }
    };

    fetch('navbar.html')
        .then(res => res.text())
        .then(text => {
            navbar.innerHTML = text;
            const currentPage = window.location.pathname.split("/").pop() || "index.html";
            navbar.querySelectorAll("nav a.nav-link").forEach(link => {
                if (link.dataset.page === currentPage) {
                    link.classList.add("active");
                }
            });
        });

    if (readmeContent) fetchFile('https://raw.githubusercontent.com/RanchoDVT/Comp-V5/dev/README.md', readmeContent);
    if (changelogContent) fetchFile('https://raw.githubusercontent.com/RanchoDVT/Comp-V5/dev/changelog.md', changelogContent);

    const getLatestRelease = async (repo) => {
        try {
            const response = await fetch(`https://api.github.com/repos/${repo}/releases/latest`);
            if (!response.ok) throw new Error(`Error fetching release info: ${response.status}`);
            const data = await response.json();
            return data.tag_name;
        } catch (error) {
            console.error('Error fetching latest release:', error);
            return 'Unknown';
        }
    };

    if (configForm) {
        configForm.addEventListener('submit', async (event) => {
            event.preventDefault();
            const formData = new FormData(configForm);

            configOutput.textContent = `
                MOTOR_CONFIG
                {
                    FRONT_LEFT_MOTOR { PORT=${formData.get('front_left_port')} GEAR_RATIO=${formData.get('front_left_gear_ratio')} REVERSED=${formData.has('front_left_reversed')} }
                    FRONT_RIGHT_MOTOR { PORT=${formData.get('front_right_port')} GEAR_RATIO=${formData.get('front_right_gear_ratio')} REVERSED=${formData.has('front_right_reversed')} }
                    REAR_LEFT_MOTOR { PORT=${formData.get('rear_left_port')} GEAR_RATIO=${formData.get('rear_left_gear_ratio')} REVERSED=${formData.has('rear_left_reversed')} }
                    REAR_RIGHT_MOTOR { PORT=${formData.get('rear_right_port')} GEAR_RATIO=${formData.get('rear_right_gear_ratio')} REVERSED=${formData.has('rear_right_reversed')} }
                    INERTIAL { PORT=${formData.get('inertial_port')} }
                    Rear_Bumper { PORT=${formData.get('rear_bumper_port')} }
                    PRINTLOGO=${formData.has('print_logo')}
                    LOGTOFILE=${formData.has('log_to_file')}
                    MAXOPTIONSSIZE=${formData.get('max_options_size')}
                    POLLINGRATE=${formData.get('polling_rate')}
                    CTRLR1POLLINGRATE=${formData.get('ctrlr1_polling_rate')}
                    VERSION=${await getLatestRelease('RanchoDVT/Comp-V5')}
                }`;

            copyButton.style.display = 'inline-block';
        });
    }

    if (copyButton) {
        copyButton.addEventListener('click', () => {
            if (configOutput.textContent) {
                navigator.clipboard.writeText(configOutput.textContent)
                    .then(() => {
                        console.debug('Config copied to clipboard!');
                        copyButton.innerHTML = 'Copied! ✅';
                    })
                    .catch(err => console.error('Error copying text:', err));
            }
        });
    }

    const showPopup = async (type) => {
        let popupText = '', downloadLink = '';

        if (type === 'dev') {
            popupText = 'Thank you for downloading Comp_V3 *dev*! This download link goes to the Github API. This is the source code. <br> <br> You will need my Custom SDK to use this. Check out my other download in the navbar.';
            downloadLink = 'https://github.com/RanchoDVT/Comp-V5/archive/refs/heads/dev.zip';
            document.getElementById('popup-title').innerText = 'Download Comp-V3 ' + type;
        } else if (type === 'stable') {
            const latestTag = await getLatestRelease('RanchoDVT/Comp-V5');
            popupText = 'Thank you for downloading Comp_V3 stable! This download link goes to the Github API. This is the source code. <br> <br> You will need my Custom SDK to use this. Check out my other download in the navbar.';
            downloadLink = `https://github.com/RanchoDVT/Comp-V5/archive/refs/tags/${latestTag}.zip`;
            document.getElementById('popup-title').innerText = 'Download Comp-V3 ' + type + ' ' + latestTag;
        } else if (type === 'sdk') {
            const latestTag = await getLatestRelease('RanchoDVT/Vex-SDK');
            popupText = 'Thank you for downloading my custom SDK. This is unofficial and in no way affiliated, endorsed, supported, or created by VEX Robotics. <br> <br> You will need this to install my Custom SDK (This) to use my Comp_V3 Program. This modifies Vex\'s robotics extension, so PLEASE don\'t go to them if you have problems with this. Please contact me. <br> <br>There is a PowerShell script for this to make it easier: ';
            popupText += '<a href="https://minhaskamal.github.io/DownGit/#/home?url=https://github.com/RanchoDVT/Vex-SDK/blob/dev/Vex-SDK.updater.ps1">Powershell download</a>';
            document.getElementById('popup-title').innerText = 'Download Custom ' + type + ' ' + latestTag;
            downloadLink = `https://github.com/RanchoDVT/Vex-SDK/archive/refs/tags/${latestTag}.zip`;
        }

        document.getElementById('popup-text').innerHTML = popupText; // Use innerHTML to render HTML content
        document.getElementById('download-link').href = downloadLink;
        document.getElementById('popup').classList.add('active');
        document.getElementById('overlay').classList.add('active');
    };

    window.showPopup = showPopup;

});

function hidePopup() {
    document.getElementById('popup').classList.remove('active');
    document.getElementById('overlay').classList.remove('active');
}
