import express from 'express';
import bodyParser from 'body-parser';
import fs from 'fs';

const app = express();

app.use(bodyParser.text({type: '*/*'}));

const ip = 'localhost';
const PORT = 8080;

app.listen(PORT, () => {
    console.log(`server working on port ${PORT}`);
})

app.post('/api/file', (request, response) => {
    const text = request.body;
    console.log(text);
    try {
        fs.writeFileSync('userData.txt', text);
        response.sendStatus(200);
    } catch (err) {
        console.error(err)
        response.sendStatus(500);
    }
});