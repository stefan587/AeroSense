const sensorURL = 'http://192.168.1.***/data';

function mapValue(value, fromLow, fromHigh, toLow, toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

function updateRing(elementId, value, minValue, maxValue, goodThreshold, moderateThreshold) {
    const element = document.getElementById(elementId);
    const percentage = mapValue(value, minValue, maxValue, 0, 100);
    
    let color;
    if (percentage <= goodThreshold) {
        color = '#3949ab';
    } else if (percentage <= moderateThreshold) {
        color = '#5c6bc0';
    } else {
        color = '#7986cb';
    }
    
    element.style.background = `conic-gradient(${color} 0%, ${color} ${percentage}%, #9fa8da ${percentage}%, #9fa8da 100%)`;
}

function fetchSensorData() {
    fetch(sensorURL)
        .then(response => response.json())
        .then(data => {
            document.getElementById('air-quality').textContent = data.airQuality;
            document.getElementById('temperature').textContent = data.temperature;
            document.getElementById('humidity').textContent = data.humidity;
            document.getElementById('co2').textContent = data.co2;
            
            updateRing('air-quality-ring', data.airQuality, 0, 1000, 30, 70);
            updateRing('temperature-ring', data.temperature, 10, 35, 40, 70);
            updateRing('humidity-ring', data.humidity, 0, 100, 40, 70);
            updateRing('co2-ring', data.co2, 400, 2000, 30, 70);
        })
        .catch(error => {
            console.error('Error fetching sensor data:', error);
        });
}

setInterval(fetchSensorData, 1000);
fetchSensorData();